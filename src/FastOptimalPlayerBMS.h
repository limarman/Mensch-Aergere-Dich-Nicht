#include "GamePlayer.h"
#include "CompressorR6.h"
#pragma once

/*
 fast Optimal player best moves (split) - implements GamePlayer
 Loads the 6 best move files into RAM and then works like Optimal Player split
*/
class FastOptimalPlayerBMS : public GamePlayer
{

private:

	vector<optimalMoveUnit4_r> optimalMoves[6];

	/*
	Returns the optimal move piece index for dieroll
	*/
	int getBestMovePieceIndex(uint64_t posID, int roll);

public:
	Position chooseSuccessor(Position& p, int dieroll);

	/*
	Loads the best move files with the names "filenameBase(1-6).bin" into the optimalMoves vector.
	*/
	FastOptimalPlayerBMS(string filenameBase);
	~FastOptimalPlayerBMS();
};