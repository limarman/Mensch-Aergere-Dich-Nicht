#include "GamePlayer.h"
#include <random>
#pragma once
class RandomPlayer : public GamePlayer
{

private:
	random_device rd;
	mt19937 mt; 
public:

	Position chooseSuccessor(Position& p, int dieroll);

	RandomPlayer();
	~RandomPlayer();
};

