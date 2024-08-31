#include "PythonPlayer.h"
#include <algorithm>

Position PythonPlayer::chooseSuccessor(Position& p, int dieroll)
{
	//create a copy of Position p - as we are going to sort the piece array for the player to move
	Position pNew = p;

	vector<Position> succs = p.calculateSuccessors(dieroll);

	//bestMoves file does not work if we need to pass our move (no piece moves)
	if (succs.size() == 1)
	{
		return succs[0];
	}

	int bestIndex = getBestMovePieceIndex(pNew.encodePosition(), dieroll);

	//apply the move for the best index to get the optimal succsessor

	//grab the relevant pieces
	vector<int>* playerPieces;

	//player 1's turn
	if (pNew.getTurn())
	{
		playerPieces = pNew.getPlayer(1)->getPiecePositions();

		sort((*playerPieces).begin(), (*playerPieces).end());
	}
	//player 0's turn
	else
	{
		playerPieces = pNew.getPlayer(0)->getPiecePositions();

		sort((*playerPieces).begin(), (*playerPieces).end());
	}

	bool nextTurn = (dieroll == 6 ? pNew.getTurn() : !pNew.getTurn());

	if ((*playerPieces)[bestIndex] == -1)
	{
		//promote the piece to the start square
		pNew.movePieceTo(bestIndex, 0, nextTurn);
	}
	else
	{
		//move the piece by dieroll squares
		pNew.movePieceTo(bestIndex, (*playerPieces)[bestIndex] + dieroll, nextTurn);
	}

	//check whether pNew is a legit successsor else choose successor randomly
	for (Position succ : succs)
	{
		if (pNew.isEqual(succ)) 
		{
			//it is part of the successors - can be returned
			return pNew;
		}
	}

	//return some legit successor (the GamePlayer tried to "cheat")
	return succs[0];
}

int PythonPlayer::getBestMovePieceIndex(uint64_t positionID, int dieroll)
{
	return 42;

	//query python object for move choice
}

PythonPlayer::PythonPlayer(string fileDir) 
{

}

PythonPlayer::~PythonPlayer()
{
}