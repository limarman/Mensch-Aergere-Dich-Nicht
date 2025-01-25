/*
Interface for communicating with a Gameplayer implemented in Python. (Trained NN for example)

Creates a Python "GamePlayer" object from the given file direction of the Python script.
Calls GamePlayer.getMove() for every move of Python Player
*/

#pragma once
#include "GamePlayer.h"
#include <mutex>
#include <thread>
#include <Python.h>
#include <memory>  // for unique_ptr


// Define visibility macro
#ifdef _WIN32
#  define PYTHON_PLAYER_EXPORT __declspec(dllexport) // For Windows
#else
#  define PYTHON_PLAYER_EXPORT __attribute__((visibility("default"))) // For Unix-like systems
#endif


// Class for managing sub-interpreters
class SubInterpreterState {
public:
    PyThreadState* tstate = nullptr;  // Thread state for the sub-interpreter
    PyObject* get_move_func = nullptr; // Python function reference
    std::thread::id threadId;          // Owning thread ID

    SubInterpreterState(const char* scriptPath, PyInterpreterConfig& config);
    ~SubInterpreterState();

private:
    void initialize_and_import_function(const char* scriptPath); // Helper function
    void createNewInterpreter(PyInterpreterConfig &config); // Helper function to create a new interpreter
};

// Class to manage multiple interpreters (e.g., multiton pattern)
class InterpreterManager {
public:
    InterpreterManager(const char* scriptPath);
    ~InterpreterManager();

    SubInterpreterState* getOrCreateInterpreter();

private:
    std::mutex mutex;  // Mutex for thread-safe access
    std::vector<std::unique_ptr<SubInterpreterState>> interpreters; // List of interpreters
    const char* scriptPath;

    PyInterpreterConfig config = {
        .use_main_obmalloc = 0,
        .allow_fork = 0,
        .allow_exec = 0,
        .allow_threads = 1,
        .allow_daemon_threads = 0,
        .check_multi_interp_extensions = 1,
        .gil = PyInterpreterConfig_OWN_GIL,
    };
};

class PYTHON_PLAYER_EXPORT PythonPlayer : public GamePlayer
{
private:
	InterpreterManager interpreterManager;  // Manages sub-interpreters
	const char* scriptPath;                 // Path to the Python script
	
	PyObject* positionToPyDict(Position& p);
	Position query(Position& p, int dieroll, vector<int>* indices, vector<Position>* succs);

public:
	
	Position chooseSuccessor(Position& p, int dieroll);

	PythonPlayer(const char* fileDir);
	~PythonPlayer();

};