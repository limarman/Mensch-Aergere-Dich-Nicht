#include "GamePlayer.h"
#pragma once
class ImprovedSimplePlayer : public GamePlayer
{

private: 

	int evaluateSuccessor(Position& p, bool view);

public:
	Position chooseSuccessor(Position& p, int dieroll);
	ImprovedSimplePlayer();
	~ImprovedSimplePlayer();
};

