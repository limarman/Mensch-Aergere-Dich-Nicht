#include "Player.h"
#include <algorithm>

Player::Player() : pieces(pieceNumber)
{
	for (int i = 0; i < pieceNumber - 1; i++)
	{
		pieces[i] = -1; //pieces still in the box out of game
	}

	//one piece on the starting square
	pieces[pieceNumber - 1] = 0;
}

Player::Player(int assigned_pieces[]) : pieces(pieceNumber)
{
	for (int i = 0; i < pieceNumber; i++) 
	{
		pieces[i] = assigned_pieces[i];
	}
}

Player::Player(vector<int> assigned_pieces) : pieces(pieceNumber)
{
	for (int i = 0; i < pieceNumber; i++)
	{
		pieces[i] = assigned_pieces[i];
	}
}

void Player::capturePieceOn(int square)
{
	for (int i = 0; i < pieceNumber; i++)
	{
		if (pieces[i] == square)
		{
			pieces[i] = -1;
		}
	}
}

void Player::movePieceOn(int pieceID, int square)
{
	pieces[pieceID] = square;
}

void Player::movePieceByOne(int index)
{
	pieces[index]++;
}

bool Player::hasPieceOn(int square)
{
	for (int numbers : pieces)
	{
		if (numbers == square)
		{
			return true;
		}
	}

	return false;
}

bool Player::hasToFreeASquare(int diceroll)
{
	bool b = false, a = false, pieceOnDiceroll = false;
	for (int positions : pieces)
	{
		if (positions == -1)
		{
			b = true;
		}
		else if (positions == 0)
		{
			a = true;
		}
		else if (positions == diceroll)
		{
			return false;
		}
	}
	return b && a && !pieceOnDiceroll;

}

bool Player::hasFinished()
{
	for (int piece : pieces)
	{
		if (piece < squareNumber)
		{
			return false;
		}
	}
	return true;
}

int Player::getIndexAtPosition(int position)
{
	for (int i = 0; i < pieceNumber; i++)
	{
		if (pieces[i] == position)
		{
			return i;
		}
	}

	//no Index found
	return -1;
}

int Player::getPositionAtIndex(int index)
{
	return pieces[index];
}

vector<int>* Player::getPiecePositions()
{
	return &pieces;
}

int Player::encodeFinishCombination(int start, int end)
{
	uint64_t id = 0;

	for (int i = 0; i < end - start + 1; i++)
	{
		int index = pieces[i + start] - squareNumber; //dialing back to interval [0,pieceNumber]
		int rest = pieceNumber - index - 1;
		id += binomialCoefficient(rest, end - start + 1 - i);
	}

	return static_cast<int>(id);
}

uint64_t Player::encodeInGameCombinationFirst(int start, int end)
{
	uint64_t id = 0;

	for (int i = 0; i < end - start + 1; i++)
	{
		int index = pieces[i + start];
		int rest = squareNumber - index - 1;
		id += binomialCoefficient(rest, end - start + 1 - i);
	}

	return id;
}

uint64_t Player::encodeInGameCombinationSecond(int start, int end, vector<int> firstPositions, int startFirst, int endFirst)
{
	int firstPiecesInGame = endFirst - startFirst + 1;
	int secondPiecesInGame = end - start + 1;

	//counting how many adversary's pieces are between start and piece position
	vector<int> piecesBefore(secondPiecesInGame);

	//inversed counting by decrementing if not the case. See below where it comes in handy.
	for (int i = 0; i < secondPiecesInGame; i++)
	{
		piecesBefore[i] = firstPiecesInGame; //maximal amount of pieces before 
	}

	for (int i = 0; i < firstPiecesInGame; i++)
	{
		int toggled = toggleSquareSpace(firstPositions[startFirst + i]);
		for (int j = 0; j < secondPiecesInGame; j++)
		{
			if (pieces[start + j] < toggled)
			{
				piecesBefore[j]--;
			}
			else
			{
				continue; //here it comes in handy.. no need to look at the rest, as there will be no change either
			}
		}
	}

	uint64_t id = 0;

	for (int i = 0; i < secondPiecesInGame; i++)
	{
		int index = pieces[i + start] - piecesBefore[i];
		int rest = squareNumber - index - 1 - firstPiecesInGame;
		id += binomialCoefficient(rest, secondPiecesInGame - i);
	}

	return id;
}

int Player::toggleSquareSpace(int square)
{
	//SafeZone - cannot be toggled
	if (square >= squareNumber)
	{
		return -2; //as -1 is for the B-squares
	}

	square += squareNumber / 2;
	if (square >= squareNumber)
	{
		square -= squareNumber;
	}

	return square;
}

bool Player::isEqual(Player& player) 
{
	sort(this->pieces.begin(), this->pieces.end());
	sort(player.pieces.begin(), player.pieces.end());

	for (int i = 0; i < pieceNumber; i++) 
	{
		if (this->pieces[i] != player.pieces[i]) 
		{
			return false;
		}
	}

	return true;
}

int Player::countMovesToGoal()
{
	//count the number of squares player has to cover
	int res = 0;
	for (int i = 0; i < pieceNumber; i++) 
	{
		//piece has to get out of box with a roll of 6
		res += squareNumber + i + 6;
	}

	//iterate through piecearray and calculate how many squares are left
	for (int piece : pieces) 
	{
		//if piece on square -1 it has not passed any squares yet
		if (piece != -1) 
		{
			res -= (piece + 6);
		}
	} 

	return res;
}
