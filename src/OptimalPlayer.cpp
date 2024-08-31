#include "OptimalPlayer.h"
#include "IOResults.h"


Position OptimalPlayer::chooseSuccessor(Position& p, int dieroll)
{
	vector<Position> succs = p.calculateSuccessors(dieroll);

	//look up winning probs for every possible successor and choose the maximal
	int bestPosition = 0;
	if (dieroll == 6) //special case as winning probablitites do not switch between Max and Min player
	{
		float maxProbability = 0;
		for (int i = 0; i < succs.size(); i++)
		{
			uint64_t encoded = succs[i].encodePosition();
			if ((*referenceProbs)[encoded] > maxProbability)
			{
				maxProbability = (*referenceProbs)[encoded];
				bestPosition = i;
			}
		}
	}
	else //probability view switch has to be considered
	{
		float maxProbability = 0;
		for (int i = 0; i < succs.size(); i++)
		{
			uint64_t encoded = succs[i].encodePosition();
			if (1 - (*referenceProbs)[encoded] > maxProbability)
			{
				maxProbability = 1 - (*referenceProbs)[encoded];
				bestPosition = i;
			}
		}
	}

	return succs[bestPosition];

}

OptimalPlayer::OptimalPlayer(vector<float>* probabilities) 
{
	this->referenceProbs = probabilities;
}

//OptimalPlayer::OptimalPlayer() : referenceProbs(numberOfPositions)
//{
//
//	//load probabilitites from backUpFile
//	string loadedFile = "results/ResultsC.bin";
//
//	//load from file
//	loadProbabilities(referenceProbs, loadedFile);
//}


OptimalPlayer::~OptimalPlayer()
{
}
