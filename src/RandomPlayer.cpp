#include "RandomPlayer.h"



Position RandomPlayer::chooseSuccessor(Position& p, int dieroll)
{
	vector<Position> succs = p.calculateSuccessors(dieroll);

	uniform_int_distribution<int> dist(0, succs.size()-1);


	int bestPosition = dist(mt);

	return succs[bestPosition];
}

RandomPlayer::RandomPlayer()
{
	mt.seed(rd());
}


RandomPlayer::~RandomPlayer()
{
}
