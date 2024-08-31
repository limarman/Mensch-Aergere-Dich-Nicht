#pragma once
#include "GamePlayer.h"

/*
Optimal player best moves - implements GamePlayer
chooses the best move according to bestMove file (and correspoding compressor)
*/
class OptimalPlayerBM : public GamePlayer
{
private:
	//true means the r6 compressor has been used,
	//false means the default compressor has been used for generating the bestmove file(s)
	bool splittedBestMoves; 
public:
	Position chooseSuccessor(Position& p, int dieroll);
	OptimalPlayerBM(bool splittedBestMoves);
	~OptimalPlayerBM();
}; 
