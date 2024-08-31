#include "ImprovedSimplePlayer.h"
#include <climits>


Position ImprovedSimplePlayer::chooseSuccessor(Position& p, int dieroll)
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

int ImprovedSimplePlayer::evaluateSuccessor(Position& p, bool view)
{
	int evaluation = 0;
	int id = view ? 1 : 0;

	vector<int>* piecesView = p.getPlayer(id)->getPiecePositions();
	vector<int>* piecesOpponent = p.getPlayer(1 - id)->getPiecePositions();

	//add the squared piece positions of player with id together
	for (int i = 0; i < pieceNumber; i++)
	{
		evaluation += (*piecesView)[i] * (*piecesView)[i];
	}

	//consider relative positions of pieces
	for (int i = 0; i < pieceNumber; i++)
	{
		for (int j = 0; j < pieceNumber; j++)
		{
			if (Player::toggleSquareSpace((*piecesView)[i]) < (*piecesOpponent)[j]) 
			{
				evaluation += 5000; //if piece behind than +50000
			}
		}
	}

	//add for every opponnent's captured piece a huge sum, so that this will always be the prio
	for (int i = 0; i < pieceNumber; i++)
	{
		if ((*piecesOpponent)[i] == -1)
		{
			evaluation += 50000;
		}
	}

	return evaluation;
}

ImprovedSimplePlayer::ImprovedSimplePlayer()
{
}


ImprovedSimplePlayer::~ImprovedSimplePlayer()
{
}
