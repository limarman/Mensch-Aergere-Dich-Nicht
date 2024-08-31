#include "FastOptimalPlayerBMS.h"
#include "IOResults.h"
#include <algorithm>

Position FastOptimalPlayerBMS::chooseSuccessor(Position& p, int dieroll)
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

	return pNew;
}

FastOptimalPlayerBMS::FastOptimalPlayerBMS(string filenameBase)
{
	//resize the vectors to size ceil(numberOfPositions/4)
	uint64_t numberOfUnits = (numberOfPositions % 4 == 0) ?
		numberOfPositions / 4 : numberOfPositions / 4 + 1;

	for (int i = 0; i < 6; i++) 
	{
		optimalMoves[i].resize(numberOfUnits);
	}

	//load bestMoves into Array
	for (int i = 0; i < 6; i++) 
	{
		string filename = filenameBase + std::to_string(i + 1) + ".bin";
		loadBestMoves_r(optimalMoves[i], filename);
	}
}

FastOptimalPlayerBMS::~FastOptimalPlayerBMS()
{
}

int FastOptimalPlayerBMS::getBestMovePieceIndex(uint64_t posID, int roll)
{
	//positions are saved into units of four positions
	//therefore unit number i holds the information for positionID 4i, 4i+1, 4i+2, 4i+3
	uint64_t unitID = posID / 4;

	optimalMoveUnit4_r unit = optimalMoves[roll - 1][unitID];

	optimalMoveBits_r bits[4];
	CompressorR6::decodeOptimalMoveUnit4_r(unit, bits); //decode unit in its optimal Move bits

	int optimalPieceIndex = 0;
	optimalPieceIndex |= (bits[posID % 4] & 0b00000011); //picking out the bits which are are relevant in the unit


	return optimalPieceIndex;
}
