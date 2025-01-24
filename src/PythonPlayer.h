/*
Interface for communicating with a Gameplayer implemented in Python. (Trained NN for example)

Creates a Python "GamePlayer" object from the given file direction of the Python script.
Calls GamePlayer.getMove() for every move of Python Player
*/

#include "GamePlayer.h"
#include <mutex>
#include <thread>
#include <Python.h>
#include <memory>  // for unique_ptr
// #include <pybind11/pybind11.h> // Include pybind11 headers
// #include <pybind11/embed.h>    // Only needed if embedding Python
#pragma once

namespace py = pybind11; // Define the namespace

// Define visibility macro
#ifdef _WIN32
#  define PYTHON_PLAYER_EXPORT __declspec(dllexport) // For Windows
#else
#  define PYTHON_PLAYER_EXPORT __attribute__((visibility("default"))) // For Unix-like systems
#endif

class PYTHON_PLAYER_EXPORT PythonPlayer : public GamePlayer
{
private:
	struct SubInterpreterState {
        PyThreadState* tstate = nullptr; // Sub-interpreter thread state
        py::object get_move_func;        // Function reference
		std::thread::id threadId;        // Owning thread ID

        SubInterpreterState(const char* scriptPath);
        ~SubInterpreterState();
    };

	std::mutex mutex;
    std::vector<std::unique_ptr<SubInterpreterState>> interpreters;
    const char* scriptPath;

    SubInterpreterState* getOrCreateInterpreter();

	// int getBestMovePieceIndex(uint64_t positionID, int dieroll);
	// py::object get_move_func;
	// py::scoped_interpreter guard;
	py::dict positionToPyDict(Position& p);
	Position query(Position& p, int dieroll, vector<int>* indices, vector<Position>* succs);

public:
	
	Position chooseSuccessor(Position& p, int dieroll);

	PythonPlayer(const char* fileDir);

	~PythonPlayer();

};