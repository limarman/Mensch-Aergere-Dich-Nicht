#include "Position.h"
#pragma once
class GamePlayer
{
public:
	virtual Position chooseSuccessor(Position& p, int dieroll) = 0;
};

