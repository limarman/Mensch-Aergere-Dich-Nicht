#include "PythonPlayer.h"
#include <algorithm>
#include <iostream>

Position PythonPlayer::chooseSuccessor(Position& p, int dieroll)
{
	//create a copy of Position p - as we are going to sort the piece arrays
	Position pNew = p;

	//std::cout << "choose succ called" << endl;

	//bringing position to sorted normalform
	pNew.toSortedNormalForm();

	//std::cout << "Sorted To NormalForm" << endl;

	//grab player to move
	int playerToMove = 0;
	if (pNew.getTurn())
	{
		playerToMove = 1;
	}

	vector<Position> succs = p.calculateSuccessors(dieroll);

	//std::cout << "Compute successors" << endl;

	// If there is only one possible successor, we do not need to query the python script
	if (succs.size() == 1)
	{
		return succs[0];
	}

	//std::cout << "More than one possible succ" << endl;

	// find out the indices to the successors
	vector<int> indices(succs.size());

	for(size_t i; i < indices.size(); i++)
	{
		for(int changedPiecePos = 0; changedPiecePos < pieceNumber; changedPiecePos++)
		{
			if (pNew.getPlayer(playerToMove)->getPositionAtIndex(changedPiecePos) !=
				succs[i].getPlayer(playerToMove)->getPositionAtIndex(changedPiecePos))
				{
					indices[i] = changedPiecePos;
					break;
				}
		}
	}

	// std::cout << "Query Python function" << endl;

	//query the python model
	return this->query(p, dieroll, &indices, &succs);
}

/*
* The query function that accesses the python subinterpreter
*/
Position PythonPlayer::query(Position& p, int dieroll, vector<int>* indices, vector<Position>* succs)
{	

	// Ensure the correct sub-interpreter is used for this thread
    SubInterpreterState* sub = interpreterManager.getOrCreateInterpreter();

    // Save the current thread state and switch to the sub-interpreter's thread state
    PyThreadState* saved_tstate = PyThreadState_Swap(sub->tstate);
	
	// OPTIONAL but maybe reasonable:
	// sort the indices in ascending order (together with the succs)
	// bring the successors to sorted normalform
	// sort indices in ascending order and reorder successors accordingly
	vector<int> sort_indices(indices->size());
	for(size_t i = 0; i < sort_indices.size(); ++i) {
		sort_indices[i] = i;
	}
	
	sort(sort_indices.begin(), sort_indices.end(), [indices](int i1, int i2) {
		return (*indices)[i1] < (*indices)[i2];
	});

	// reorder indices and successors based on sorted order
	vector<int> sorted_indices;
	vector<Position> sorted_succs;
	for(size_t i = 0; i < sort_indices.size(); ++i) {
		sorted_indices.push_back((*indices)[sort_indices[i]]);
		sorted_succs.push_back((*succs)[sort_indices[i]]);
	}

	// update input vectors
	*indices = sorted_indices;
	*succs = sorted_succs;

	// bring successors to sorted normal form
	for(auto& succ : *succs) {
		succ.toSortedNormalForm();
	}

	// convert the position and the succs into a dataformat that can be passed to Python
	// create dictionary for current position
    PyObject* pos_dict = positionToPyDict(p);
    
    // Create a list of successor dictionaries
    PyObject* succ_list = PyList_New(succs->size());
    for (size_t i = 0; i < succs->size(); ++i) {
        PyObject* succ_dict = positionToPyDict((*succs)[i]);
        PyList_SET_ITEM(succ_list, i, succ_dict);
    }

    // Convert indices to a Python list
    PyObject* indices_list = PyList_New(indices->size());
    for (size_t i = 0; i < indices->size(); ++i) {
        PyList_SET_ITEM(indices_list, i, PyLong_FromLong((*indices)[i]));
    }

    // Call Python function with parameters
    PyObject* result = PyObject_CallFunctionObjArgs(sub->get_move_func, pos_dict, PyLong_FromLong(dieroll), indices_list, succ_list, nullptr);

    // Convert the result to C++ int
    int chosen_index = PyLong_AsLong(result);

	// error handling
	auto it = std::find(indices->begin(), indices->end(), chosen_index);
	if (it == indices->end()) {
		throw std::runtime_error("Python player returned invalid move index");
	}

	// choose position
	size_t pos = std::distance(indices->begin(), it);
	
	// Restore the original thread state after the function call
    PyThreadState_Swap(saved_tstate);

	// return corresponding successor
	return (*succs)[pos];

	// return (*succs)[0];
}

PyObject* PythonPlayer::positionToPyDict(Position& p) {
	// Create a new Python dictionary
    PyObject* pos_dict = PyDict_New();

    // Convert player 0's piece positions to a Python list
    std::vector<int> player0_positions = *(p.getPlayer(0)->getPiecePositions());
    PyObject* py_player0_positions = PyList_New(player0_positions.size());
    for (size_t i = 0; i < player0_positions.size(); ++i) {
        PyList_SET_ITEM(py_player0_positions, i, PyLong_FromLong(player0_positions[i]));
    }

    // Add player 0's positions to the dictionary
    PyDict_SetItemString(pos_dict, "player0", py_player0_positions);
    Py_DECREF(py_player0_positions);  // Decrement reference as the list is now owned by the dict

    // Convert player 1's piece positions to a Python list
    std::vector<int> player1_positions = *(p.getPlayer(1)->getPiecePositions());
    PyObject* py_player1_positions = PyList_New(player1_positions.size());
    for (size_t i = 0; i < player1_positions.size(); ++i) {
        PyList_SET_ITEM(py_player1_positions, i, PyLong_FromLong(player1_positions[i]));
    }

    // Add player 1's positions to the dictionary
    PyDict_SetItemString(pos_dict, "player1", py_player1_positions);
    Py_DECREF(py_player1_positions);  // Decrement reference as the list is now owned by the dict

    // Set the "turn" value as a boolean in the dictionary
    PyObject* py_turn = PyBool_FromLong(p.getTurn() ? 1 : 0);
    PyDict_SetItemString(pos_dict, "turn", py_turn);
    Py_DECREF(py_turn);  // Decrement reference as the boolean is now owned by the dict

    // Return the populated Python dictionary
    return pos_dict;
}

PythonPlayer::PythonPlayer(const char* fileDir) : scriptPath(fileDir), interpreterManager(fileDir)
{	
	
}

PythonPlayer::~PythonPlayer()
{

}

/*
* SUBINTERPRETER IMPLEMENTATIONS
*/

// Constructor: Initialize sub-interpreter and import the Python function
SubInterpreterState::SubInterpreterState(const char* scriptPath, PyInterpreterConfig &config) {

    // Create a new sub-interpreter
    createNewInterpreter(config);

    if (!tstate) {
        std::cerr << "Failed to create sub-interpreter." << std::endl;
        //PyGILState_Release(gstate);
        throw std::runtime_error("Sub-interpreter creation failed.");
    }

    // Switch to the sub-interpreter
    PyThreadState_Swap(tstate);

    // Initialize and import the function from the Python module
    initialize_and_import_function(scriptPath);

    // Save the current thread ID
    threadId = std::this_thread::get_id();

    PyEval_ReleaseThread(PyThreadState_Get());
}

// Destructor: Clean up the sub-interpreter
SubInterpreterState::~SubInterpreterState() {
    // Acquire the GIL and switch to the sub-interpreter
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyThreadState_Swap(tstate);

    // Clean up the Python objects (e.g., `get_move_func`)
    if (get_move_func) {
        Py_DECREF(get_move_func);
    }

    // Close the sub-interpreter
    Py_EndInterpreter(tstate);

    // Release the GIL
    PyGILState_Release(gstate);
}

// Helper function: Initialize the sub-interpreter and import the function
void SubInterpreterState::initialize_and_import_function(const char* scriptPath) {
    // Add script directory to Python's sys.path
    PyObject* sys_module = PyImport_ImportModule("sys");
    if (!sys_module) {
        PyErr_Print();
        throw std::runtime_error("Failed to import sys module.");
    }

    PyObject* sys_path = PyObject_GetAttrString(sys_module, "path");
    if (!sys_path) {
        Py_DECREF(sys_module);
        PyErr_Print();
        throw std::runtime_error("Failed to get sys.path.");
    }

    std::string dir = std::string(scriptPath);
    size_t last_slash = dir.find_last_of("/\\");
    if (last_slash != std::string::npos) {
        std::string script_dir = dir.substr(0, last_slash);
        PyObject* py_script_dir = PyUnicode_FromString(script_dir.c_str());
        PyList_Append(sys_path, py_script_dir);  // Append the script directory to sys.path
        Py_DECREF(py_script_dir);
    }

    Py_DECREF(sys_module);
    Py_DECREF(sys_path);

    // Extract module name from the path
    std::string module_name = dir.substr(last_slash + 1);
    if (module_name.size() >= 3 && module_name.substr(module_name.size() - 3) == ".py") {
        module_name = module_name.substr(0, module_name.size() - 3);
    }

    // Import the Python module
    PyObject* player_module = PyImport_ImportModule(module_name.c_str());
    if (!player_module) {
        PyErr_Print();
        throw std::runtime_error("Failed to import Python module.");
    }

    // Retrieve the get_move function from the module
    get_move_func = PyObject_GetAttrString(player_module, "get_move");
    if (!get_move_func || !PyCallable_Check(get_move_func)) {
        Py_DECREF(player_module);
        PyErr_Print();
        throw std::runtime_error("Failed to get 'get_move' function from the module.");
    }

    Py_DECREF(player_module);  // Decrement reference to the module object
}

void SubInterpreterState::createNewInterpreter(PyInterpreterConfig &config){

    Py_NewInterpreterFromConfig(&tstate, &config);

}
/*
* INTERPRETER MANAGER IMPLEMENTATIONS
*/

// Constructor: Initialize with the script path
InterpreterManager::InterpreterManager(const char* scriptPath) : scriptPath(scriptPath) {
    // Initialize the Python interpreter if it's not already initialized
    if (!Py_IsInitialized()) {
        Py_Initialize();  // Initialize the global interpreter lock (GIL)
    }

	PyEval_ReleaseThread(PyThreadState_Get());

}

// Destructor: Clean up all sub-interpreters
InterpreterManager::~InterpreterManager() {
    // Acquire the GIL and clean up all sub-interpreters
    PyGILState_STATE gstate = PyGILState_Ensure();
    
    for (auto& interpreter : interpreters) {
        interpreter.reset();  // Destructors will be called for each `SubInterpreterState`
    }

    // Release the GIL
    PyGILState_Release(gstate);
}

// Get or create a sub-interpreter for the current thread
SubInterpreterState* InterpreterManager::getOrCreateInterpreter() {
    std::lock_guard<std::mutex> lock(mutex);  // Ensure thread safety with a mutex

    // Check if the current thread already has a sub-interpreter
    std::thread::id current_thread_id = std::this_thread::get_id();
    //std::cout << "Calling for Subinterpreter" << " Thread ID " << current_thread_id << endl;
    for (auto& interpreter : interpreters) {
        if (interpreter->threadId == current_thread_id) {
            return interpreter.get();
        }
    }

    // If no interpreter exists for this thread, create a new one
    std::unique_ptr<SubInterpreterState> new_interpreter(new SubInterpreterState(scriptPath, config));

    // Save the new interpreter in the list
    interpreters.push_back(std::move(new_interpreter));

    // Return the new interpreter
    return interpreters.back().get();
}