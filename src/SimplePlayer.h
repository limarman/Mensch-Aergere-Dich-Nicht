#include "GamePlayer.h"
#pragma once
/*
Take or First Piece First
*/
class SimplePlayer : public GamePlayer
{
private: 
	int evaluateSuccessor(Position& p, bool view);

public:
	Position chooseSuccessor(Position& p, int dieroll);

	SimplePlayer();
	~SimplePlayer();
};

