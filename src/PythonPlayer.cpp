#include "PythonPlayer.h"
#include <algorithm>
#include <iostream>

struct GlobalPython {
    PyInitialize();
};
static GlobalPython global_python;  // Initialize once


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

Position PythonPlayer::query(Position& p, int dieroll, vector<int>* indices, vector<Position>* succs)
{	

	// Get or create the sub-interpreter for this thread
    SubInterpreterState* sub = getOrCreateInterpreter();

    // Save the current thread state (e.g., the main interpreter)
    PyThreadState* old_tstate = PyThreadState_Get();

    // Switch to the sub-interpreter's thread state
    PyEval_RestoreThread(sub->tstate);


	return (*succs)[0];
	// std::cout << "query Python Player";
	
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
	py::dict pos_dict = positionToPyDict(p);


	// create list of successor dictionaries
	py::list succ_list;
	for(auto& succ : *succs) {
		py::dict succ_dict = positionToPyDict(succ);
		succ_list.append(succ_dict);
	}

	// // convert indices to python list
	py::list indices_list = py::cast(*indices);
	
	// call Python function with parameters
	py::object result = sub->get_move_func(pos_dict, dieroll, indices_list, succ_list);
	
	// convert result to C++ int
	int chosen_index = result.cast<int>();

	// error handling
	auto it = std::find(indices->begin(), indices->end(), chosen_index);
	if (it == indices->end()) {
		throw std::runtime_error("Python player returned invalid move index");
	}

	// choose position
	size_t pos = std::distance(indices->begin(), it);
	
	// return corresponding successor
	return (*succs)[pos];

	// return (*succs)[0];
}

py::dict PythonPlayer::positionToPyDict(Position& p) {
	py::dict pos_dict;
	pos_dict["player0"] = py::cast(p.getPlayer(0)->getPiecePositions());
	pos_dict["player1"] = py::cast(p.getPlayer(1)->getPiecePositions());
	pos_dict["turn"] = py::cast(p.getTurn());
	return pos_dict;
}

PythonPlayer::SubInterpreterState::SubInterpreterState(const char* scriptPath) : threadId(std::this_thread::get_id()) {  // Track owning thread
    // Release the GIL of the main interpreter
    py::gil_scoped_release release;

    // Save the main interpreter's thread state
    PyThreadState* main_tstate = PyThreadState_Get();

    // Create a new sub-interpreter
    tstate = Py_NewInterpreter();
    if (!tstate) {
        throw std::runtime_error("Failed to create sub-interpreter");
    }

    // Now in the sub-interpreter's context
    {
        // Acquire the GIL for this sub-interpreter
        py::gil_scoped_acquire acquire;

        // Add script directory to Python path
        py::module sys = py::module::import("sys");
        std::string dir = std::string(scriptPath);
        size_t last_slash = dir.find_last_of("/\\");
        if (last_slash != std::string::npos) {
            sys.attr("path").attr("append")(dir.substr(0, last_slash));
        }

        // Import the Python module
        std::string module_name = dir.substr(last_slash + 1);
        if (module_name.size() >= 3 && 
            module_name.substr(module_name.size() - 3) == ".py") {
            module_name = module_name.substr(0, module_name.size() - 3);
        }
        py::module player_module = py::module::import(module_name.c_str());

        // Store the Python function reference
        get_move_func = player_module.attr("get_move");
    }

    // Restore the main interpreter's thread state
    PyEval_RestoreThread(main_tstate);
}

PythonPlayer::SubInterpreterState::~SubInterpreterState() {
    if (tstate) {
        // Save the current thread state
        PyThreadState* old_tstate = PyThreadState_Get();

        // Switch to this sub-interpreter's thread state
        PyEval_RestoreThread(tstate);

        // Destroy the sub-interpreter
        Py_EndInterpreter(tstate);

        // Restore the original thread state
        PyEval_RestoreThread(old_tstate);
    }
}

PythonPlayer::SubInterpreterState* PythonPlayer::getOrCreateInterpreter() {
    std::lock_guard<std::mutex> lock(mutex);  // Thread-safe access
    std::thread::id current_id = std::this_thread::get_id();

    // Check if this thread already has a sub-interpreter
    for (auto& state : interpreters) {
        if (state->threadId == current_id) {
            return state.get();
        }
    }

    // Create a new sub-interpreter for this thread
    interpreters.push_back(
        std::unique_ptr<SubInterpreterState>(new SubInterpreterState(scriptPath))
    );
    return interpreters.back().get();
}

PythonPlayer::PythonPlayer(const char* fileDir) : scriptPath(fileDir)
{	
	// create the first thread
}

PythonPlayer::~PythonPlayer()
{
	//py::finalize_interpreter(); // Stop Python interpreter
}