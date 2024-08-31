#include "SimplePlayer.h"
#include <climits>


Position SimplePlayer::chooseSuccessor(Position& p, int dieroll) 
{
	vector<Position> succs = p.calculateSuccessors(dieroll);

	int bestPosition = 0;
	int maxEvaluation = INT_MIN;

	for (int i = 0; i < succs.size(); i++)
	{
		int evaluation = evaluateSuccessor(succs[i], p.getTurn());
		if (evaluation > maxEvaluation) 
		{
			maxEvaluation = evaluation;
			bestPosition = i;
		}
	}

	return succs[bestPosition];
}

int SimplePlayer::evaluateSuccessor(Position& p, bool view) 
{
	int evaluation = 0;
	int id = view ? 1 : 0;

	vector<int>* piecesView = p.getPlayer(id)->getPiecePositions();
	vector<int>* piecesOpponent = p.getPlayer(1-id)->getPiecePositions();

	//add the squared piece positions of player with id together
	for (int i = 0; i<pieceNumber; i++) 
	{
		evaluation += (*piecesView)[i] * (*piecesView)[i];
	}

	//add for every opponnent's captured piece a huge sum, so that this will always be the prio
	for (int i = 0; i < pieceNumber; i++) 
	{
		if ((*piecesOpponent)[i] == -1) 
		{
			evaluation += 10000;
		}
	}

	return evaluation;
}

SimplePlayer::SimplePlayer()
{
}


SimplePlayer::~SimplePlayer()
{
}
