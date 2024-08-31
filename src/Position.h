/*
*/

#ifndef POSITION_H
#define POSITION_H

#include "Player.h"
#include <string>

class Position
{
private:
	bool turn;
	Player players[2];

	vector<Position> calculateSuccessors6();
	vector<Position> calculateSuccessorsBasic(int diceroll);



	/**
	 * Encodes the distribution of pieces of the position into the distribution ID.
	 * @param inGameA - number of pieces in game of player to move
	 * @param inGameB - number of pieces in game of player not to move
	 * @param inFinishA - number of pieces in finish of player to move
	 * @param inFinishB - number of pieces in finish of player not to move
	 * @return distributionID
	 */
	int encodeDistributionID(int inGameA, int inGameB, int inFinishA, int inFinishB);

public:
	Position();
	Position(Player &player0, Player &player1, bool turn);

	int static decodeDistributionID(uint64_t positionID);

	/*
	 * gives a string representation of a position given the positionID
	*/
	string static positionToString(uint64_t positionID);

	/*
	 * gives a string representation of the position.
	*/
	string toString(void);

	/**
	 * Moves the piece at the index pieceID of the player of turn in current position to the given square and changes the turn to turn.
	 * Applys capturing of adversary's pieces.
	 * Move should be possible.
	 * @param pieceID - piece to move in [0, MadN.pieceNumber-1]
	 * @param square - square to put piece on in [0, MadN.squareNumber + MadN.pieceNumber - 1]
	 * @param turn - Turn after the move has been applied to position (true = player1, false = player0)
	 */
	void movePieceTo(int pieceID, int square, bool turn);

	/*
	Calculates the successors states of the current position
	*/
	vector<Position> calculateSuccessors(int diceroll);

	/*
	side-effect of sorting the array.
	*/
	uint64_t encodePosition();

	static Position decodePosition(uint64_t positionID);

	Player* getPlayer(int playerID);

	float getPieceRatio();

	bool getTurn();

	bool isEqual(Position& pos);

	/*
	* returns whether one of the two players has already finished the game
	*/
	bool isFinished();
};

#endif
