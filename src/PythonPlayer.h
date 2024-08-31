/*
Interface for communicating with a Gameplayer implemented in Python. (Trained NN for example)

Creates a Python "GamePlayer" object from the given file direction of the Python script.
Calls GamePlayer.getMove() for every move of Python Player
*/

#include "GamePlayer.h"
#pragma once



class PythonPlayer : public GamePlayer
{
private:

	int getBestMovePieceIndex(uint64_t positionID, int dieroll);

public:
	
	Position chooseSuccessor(Position& p, int dieroll);

	PythonPlayer(string fileDir);

	~PythonPlayer();

};