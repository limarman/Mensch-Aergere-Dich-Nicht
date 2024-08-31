#pragma once
#include "GamePlayer.h"

/*
Optimal player - implements GamePlayer
chooses the best move according to the loaded reference Probabilitites
*/
class OptimalPlayer : public GamePlayer
{
private:
	//reference Probabilitties - assumed winning probabilitites
	vector<float>* referenceProbs;
public:
	Position chooseSuccessor(Position& p, int dieroll);

	/*
	assumes that pointer goes to already loaded proabilities
	*/
	OptimalPlayer(vector<float>* referenceProbs);
	~OptimalPlayer();
};

