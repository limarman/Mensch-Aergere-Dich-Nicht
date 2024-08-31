#include "Position.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include "RatioAnalyser.h"

Position::Position()
{
	turn = false;
}

Position::Position(Player &player0, Player &player1, bool turn)
{
	players[0] = player0;
	players[1] = player1;
	this->turn = turn;
}

string Position::toString()
{
	stringstream pos;
	pos  << "Player0: ";
	for (int i = 0; i < pieceNumber; i++)
	{
		pos << getPlayer(0)->getPositionAtIndex(i) << ", ";
	}
	pos << "\nPlayer1: ";
	for (int i = 0; i < pieceNumber; i++)
	{
		pos << getPlayer(1)->getPositionAtIndex(i) << ", ";
	}
	pos << "\nTurn: " << turn;
	pos << "\n";

	return pos.str();
}

void Position::movePieceTo(int pieceID, int square, bool turn)
{
	if (this->turn)
	{
		//it is player1's turn
		players[1].movePieceOn(pieceID, square);
		players[0].capturePieceOn(Player::toggleSquareSpace(square));
		this->turn = turn;
	}
	else
	{
		//it is player0's turn
		players[0].movePieceOn(pieceID, square);
		players[1].capturePieceOn(Player::toggleSquareSpace(square));
		this->turn = turn;
	}
}

vector<Position> Position::calculateSuccessors(int diceroll) 
{
	if (diceroll == 6)
	{
		return calculateSuccessors6();
	}

	//check whose turn it is
	int turnIndex = 0;
	if (turn)
	{
		turnIndex = 1;
	}


	//Wenn B Felder besetzt und A Feld besetzt -> Sonderfall
	if (players[turnIndex].hasToFreeASquare(diceroll))
	{
		int index = players[turnIndex].getIndexAtPosition(0);

		Position succ = *this;
		succ.movePieceTo(index, diceroll, !turn);

		vector<Position> successors(1);
		successors[0] = succ;
		return successors; //There will be no other option
	}

	return calculateSuccessorsBasic(diceroll);
}

uint64_t Position::encodePosition()
{
	//check whose turn it is
	int turnIndex = 0;
	if (turn)
	{
		turnIndex = 1;
	}

	//Data: First Entry is player to move, the second for the other player
	int inGame[] = { pieceNumber, pieceNumber };
	int inFinish[] = { 0,0 };
	int lastIndexOutOfGame[] = { 0,0 };
	int lastIndexInGame[] = { 0,0 };
	vector<int>* piecePositions[] = { players[turnIndex].getPiecePositions(),
		players[1 - turnIndex].getPiecePositions() };

	//Sort Player Arrays
	sort((*piecePositions[0]).begin(), (*piecePositions[0]).end());
	sort((*piecePositions[1]).begin(), (*piecePositions[1]).end());


	//Analysis of Borders and Numbers
	for (int i = 0; i < 2; i++)
	{
		//finding boundaries in pieceArray of different stones
		while (lastIndexOutOfGame[i] < pieceNumber && (*piecePositions[i])[lastIndexOutOfGame[i]] == -1)
		{
			lastIndexOutOfGame[i]++;
			inGame[i]--;
		}
		lastIndexInGame[i] = lastIndexOutOfGame[i];
		lastIndexOutOfGame[i]--;
		//all -1's have been passed

		while (lastIndexInGame[i] < pieceNumber && (*piecePositions[i])[lastIndexInGame[i]] < squareNumber)
		{
			lastIndexInGame[i]++;
		}
		lastIndexInGame[i]--;

		//setting the number of finished pieces
		inFinish[i] = pieceNumber - lastIndexInGame[i] - 1;
	}

	//		System.out.println("player0: " + lastIndexOutOfGame[0] + " - " + lastIndexInGame[0] + " inFinish: " + inFinish[0]);
	//		System.out.println("player1: " + lastIndexOutOfGame[1] + " - " + lastIndexInGame[1] + " inFinish: " + inFinish[1]);


	//Encoding of base
	int distributionID = encodeDistributionID(inGame[0], inGame[1], inFinish[0], inFinish[1]);
	//cout << "DisributionID: " << distributionID << "\n";
	uint64_t base = configurationBases[distributionID];

	//encoding offset
	int finishIDfirst, finishIDsecond;
	uint64_t gameIDfirst, gameIDsecond;

	//ids for different playfields
	finishIDfirst = players[turnIndex].encodeFinishCombination(lastIndexInGame[0] + 1, pieceNumber - 1);
	//cout << "FinishID First: " << finishIDfirst << "\n";

	finishIDsecond = players[1 - turnIndex].encodeFinishCombination(lastIndexInGame[1] + 1, pieceNumber - 1);
	//cout << "FinishID Second: " << finishIDsecond << "\n";

	gameIDfirst = players[turnIndex].encodeInGameCombinationFirst(lastIndexOutOfGame[0] + 1, lastIndexInGame[0]);
	//cout << "GameId First: " << gameIDfirst << "\n";

	gameIDsecond = players[1 - turnIndex].encodeInGameCombinationSecond(lastIndexOutOfGame[1] + 1, lastIndexInGame[1], (*piecePositions)[0],
		lastIndexOutOfGame[0] + 1, lastIndexInGame[0]);
	//cout << "GameID Second: " << gameIDsecond << "\n";



	uint64_t offset = finishIDfirst * possibilities[distributionID].setFinishFirst +
		finishIDsecond * possibilities[distributionID].setFinishSecond +
		gameIDfirst * possibilities[distributionID].setGameFirst +
		gameIDsecond;
	//cout << "base: " << base << "\n" << "offset: " << offset << "\n";
	return base + offset;
}

Position Position::decodePosition(uint64_t positionID)
{
	// ---------------------------------------------------
	// decoding the piece distribution / base
	// ---------------------------------------------------

	//searching binary for the correct base (highest entry in MadN.bases which is <= id)
	int distributionID = decodeDistributionID(positionID);

	AbstractPosition &ap = distributionIDs[distributionID];

	positionID -= configurationBases[distributionID];

	//		System.out.println("offset to work with:" + id);

	// ---------------------------------------------------
	// decoding the piece position / offset
	// ---------------------------------------------------

	//piece positions of the player
	Player player[2];
	//finish IDs
	uint64_t finishID[2];
	//inGame IDs
	uint64_t inGameID[2];

	//ID of finished pieces of the player to move
	finishID[0] = positionID / possibilities[distributionID].setFinishFirst;
	positionID = positionID % possibilities[distributionID].setFinishFirst;

	//ID of finished pieces of the player not to move
	finishID[1] = positionID / possibilities[distributionID].setFinishSecond;
	positionID = positionID % possibilities[distributionID].setFinishSecond;

	//ID of in game pieces of the player to move
	inGameID[0] = positionID / possibilities[distributionID].setGameFirst;

	//ID of in game pieces of the player not to move
	inGameID[1] = positionID % possibilities[distributionID].setGameFirst;

	for (int i = 0; i < 2; i++)
	{
		int index = 0;

		//first filling all the pieces not in game
		while (index < pieceNumber - ap.inGame[i])
		{
			player[i].movePieceOn(index, -1); 
			index++;
		}

		//then fill positions on the field (not finished but in game)
		if (i == 0) //player to move
		{
			//decode position of pieces binary
			int rest = ap.inGame[i] - ap.inFinish[i];

			int left = 0;
			int right = squareNumber - 1;

			for (; index < pieceNumber - ap.inFinish[i]; index++) //until the last index of pieces on the field
			{
				while (true)
				{
					int middle = (left + right) / 2;
					uint64_t binCof = binomialCoefficient(squareNumber - middle - 1, rest);

					if (middle == left) //only the case when interval is of size 1 or 2
					{
						if (inGameID[i] < binCof) //the left one is not the candidate
						{
							player[i].movePieceOn(index, right);
							left = right + 1;
							right = squareNumber - 1;
							inGameID[i] -= binomialCoefficient(squareNumber - middle - 2, rest);
							rest--;
							break;
						}
						else
						{
							player[i].movePieceOn(index, left);
							left++;
							right = squareNumber - 1;
							rest--;
							inGameID[i] -= binCof;
							break;
						}
					}

					if (inGameID[i] == binCof)
					{
						player[i].movePieceOn(index, middle);
						right = squareNumber - 1;
						left = middle + 1;
						rest--;
						inGameID[i] -= binCof;
						break;
					}
					else if (inGameID[i] < binCof)
					{
						left = middle;
					}
					else
					{
						right = middle;
					}

				}
			}
		}
		else //player not to move - depends on player to move!!
		{
			int n = squareNumber - (ap.inGame[0] - ap.inFinish[0]); //number of fields

			//piece-array of player to move are necessary for decoding
			//toggle all values and sort array for algorithm to work
			vector<int> inGamePiecesPlayer0(ap.inGame[0]-ap.inFinish[0]);
			//int[] inGamePiecesPlayer0 = Arrays.copyOfRange(pieces[0], MadN.pieceNumber - ap.inGame[0], MadN.pieceNumber - ap.inFinish[0]);
			for (int j = 0; j < ap.inGame[0] - ap.inFinish[0]; j++)
			{
				inGamePiecesPlayer0[j] = Player::toggleSquareSpace(player[0].getPositionAtIndex(pieceNumber-ap.inGame[0]+j));
			}
			sort(inGamePiecesPlayer0.begin(), inGamePiecesPlayer0.end());


			//decode position of pieces binary
			int rest = ap.inGame[i] - ap.inFinish[i];

			int left = 0;
			int right = n - 1;

			for (; index < pieceNumber - ap.inFinish[i]; index++) //until the last index of pieces on the field
			{
				//					System.out.println("index: " + index);
				//					System.out.println("inGameID: " + inGameID[i]);
				while (true)
				{
					int middle = (left + right) / 2;
					uint64_t binCof = binomialCoefficient(n - middle - 1, rest);
					//						System.out.println("middle: " + middle + " binCof: " + binCof);

					if (middle == left) //only the case when interval is of size 1 or 2
					{
						//							System.out.println("middle == left");
						if (inGameID[i] < binCof) //the left one is not the candidate
						{
							//								System.out.println("case binCof larger");
							player[i].movePieceOn(index, right);
							left = right + 1;
							right = n - 1;
							inGameID[i] -= binomialCoefficient(n - middle - 2, rest);
							rest--;
							break;
						}
						else
						{
							player[i].movePieceOn(index, left);
							left++;
							right = n - 1;
							rest--;
							inGameID[i] -= binCof;
							break;
						}
					}

					if (inGameID[i] == binCof)
					{
						player[i].movePieceOn(index, middle);
						right = n - 1;
						left = middle + 1;
						rest--;
						inGameID[i] -= binCof;
						break;
					}
					else if (inGameID[i] < binCof)
					{
						left = middle;
					}
					else
					{
						right = middle;
					}

				}

				//adjust in view of oppenent's pieces
				for (int j = 0; j < ap.inGame[0] - ap.inFinish[0]; j++)
				{
					if (player[i].getPositionAtIndex(index) < inGamePiecesPlayer0[j])
					{
						break;
					}
					else
					{
						player[i].movePieceByOne(index);
					}
				}
			}
		}

		//filling the finished pieces by decoding the id linearly as small input
		int inFinish = ap.inFinish[i];
		for (int j = 0; j < pieceNumber; j++)
		{
			uint64_t binCof = binomialCoefficient(pieceNumber - j - 1, inFinish);
			if (binCof <= finishID[i])
			{
				player[i].movePieceOn(index, j + squareNumber);
				index++;
				inFinish -= 1;
				finishID[i] -= binCof;
			}
		}

		//at the end all indexes of the array should be filled
	}


	return Position(player[0], player[1], false);
}

Player* Position::getPlayer(int playerID) 
{
	return &(players[playerID]);
}

vector<Position> Position::calculateSuccessors6() 
{
	//if B square occupied, if A square occupied, if A+6 square occupied -> basic case
	//if B square occupied, if A square occupied, if A+6 square not occupied -> A square has to be freed
	//if B square occupied, if A not square occupied, if A+6 square occupied -> piece from B-Square has to be liberated
	//if B square is not occupied -> basic case

	//check whose turn it is
	int turnIndex = 0;
	if (turn)
	{
		turnIndex = 1;
	}

	//find the out the case
	bool a = false, b = false, diceroll = false;
	vector<int>* piecePositions = players[turnIndex].getPiecePositions();

	for (int i = 0; i < pieceNumber; i++)
	{
		if ((*piecePositions)[i] == -1)
		{
			b = true;
		}
		else if ((*piecePositions)[i] == 0)
		{
			a = true;
		}
		else if ((*piecePositions)[i] == 6)
		{
			diceroll = true;
		}
	}

	if (b)
	{
		if (a)
		{
			if (diceroll)
			{
				//normal
				return calculateSuccessorsBasic(6);
			}
			else
			{
				//A-square has to be freed
				int index = players[turnIndex].getIndexAtPosition(0);
				Position pos = *this;
				pos.movePieceTo(index, 6, turn);
				vector<Position> successors(1);
				successors[0] = pos;
				return successors; // There will be no other option
			}
		}
		else
		{
			//piece from B-square has to be liberated
			int index = players[turnIndex].getIndexAtPosition(-1);
			Position pos = *this;
			pos.movePieceTo(index, 0, turn);
			vector<Position> successors(1);
			successors[0] = pos;
			return successors; // There will be no other option
		}
	}
	else
	{
		//normal
		return calculateSuccessorsBasic(6);
	}
}

vector<Position> Position::calculateSuccessorsBasic(int diceroll)
{
	vector<Position> successors(4);
	int nextIndex = 0;

	//check whose turn it is
	int turnIndex = 0;
	if (turn)
	{
		turnIndex = 1;
	}

	vector<int>* piecePositions = players[turnIndex].getPiecePositions();

	for (int i = 0; i < pieceNumber; i++)
	{
		if ((*piecePositions)[i] != -1)
		{
			int movePosition = (*piecePositions)[i] + diceroll;
			if (movePosition <= squareNumber + pieceNumber - 1 && !players[turnIndex].hasPieceOn(movePosition)) //move is legal 
			{
				Position succ = *this; //found successor
				succ.movePieceTo(i, movePosition, diceroll == 6 ? turn : !turn);
				successors[nextIndex] = succ;
				nextIndex++;
			}
		}
	}

	//if there is no possible move - player has to pass
	if (nextIndex == 0)
	{
		Position pass = *this;
		pass.turn = diceroll == 6 ? turn : !turn;
		successors[0] = pass;
		nextIndex++;
	}

	successors.resize(nextIndex);

	return successors;
}

int Position::decodeDistributionID(uint64_t positionID)
{
	int left = 0, right = baseNumber-1;
	int middle;
	while (true)
	{
		middle = (left + right) / 2;

		//only the case if interval is of length 1 or 2
		if (middle == left)
		{
			if (configurationBases[right] <= positionID) //the right one is the right one! :b
			{
				return right;
			}
			else
			{
				return left;
			}
		}

		if (configurationBases[middle] > positionID)
		{
			right = middle;
		}
		else
		{
			left = middle;
		}
	}
}

string Position::positionToString(uint64_t positionID)
{
	Position p = decodePosition(positionID);
	return p.toString();
}


int Position::encodeDistributionID(int inGameA, int inGameB, int inFinishA, int inFinishB)
{
	int playerAID, playerBID, base;

	playerAID = inGameA * (inGameA + 1) / 2 + inFinishA;
	playerBID = inGameB * (inGameB + 1) / 2 + inFinishB;

	base = (pieceNumber + 1) * (pieceNumber + 2) / 2;

	return playerAID * base + playerBID;
}

float Position::getPieceRatio() 
{
	int turnIndex = 0;
	if (turn) 
	{
		turnIndex = 1;
	}

	return getRatio(players[turnIndex].countMovesToGoal(), players[1-turnIndex].countMovesToGoal());
}

bool Position::isEqual(Position& pos) 
{
	//pieces of player0s and player1s have to be the same
	if (pos.turn == this->turn) 
	{
		return players[0].isEqual(pos.players[0]) && players[1].isEqual(pos.players[1]);
	}
	else //pieces of player1 and player0 have to be the same
	{
		return players[0].isEqual(pos.players[1]) && players[1].isEqual(pos.players[0]);
	}
}

bool Position::isFinished()
{
	return players[0].hasFinished() || players[1].hasFinished();
}

bool Position::getTurn() 
{
	return turn;
}
