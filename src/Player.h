/*

*/

#ifndef PLAYER_H
#define PLAYER_H

#include "MadN.h"

class Player
{

private:
	/*
	* array holding the index of the squares the pieces of Player are on. -1 is for not yet in game.
	*/
	vector<int> pieces;

public:
	/**
	* Default Constructor, initializing pieces according to official set of rules with one piece
	* on the A-Square as well as the rest on the B-Squares
	*/
	Player();

	Player(int assigned_pieces[]);

	Player(vector<int> assigned_pieces);

	/**
	 Iterates through all player's pieces and sets back (square -1) the piece which is on the given square.
	 If no piece on given square, then nothing happens.
	 square - captured square
	 */
	void capturePieceOn(int square);

	/*
	* moves piece at arrayPosition pieceID to the square square
	*/
	void movePieceOn(int pieceID, int square);

	void movePieceByOne(int index);

	bool hasPieceOn(int square);

	bool hasToFreeASquare(int diceroll);

	bool hasFinished();

	int getIndexAtPosition(int position);

	int getPositionAtIndex(int index);

	vector<int>* getPiecePositions();

	bool isEqual(Player& player);

	/*Counts the minimum number of squares a player has to roll in total to get all pieces into finish*/
	int countMovesToGoal();

	/*
	Toggles the square space between two different players by simple addition and modulo calculation
	*/
	int static toggleSquareSpace(int square);

	//encoding operations

	/**
	*Encodes the combination of pieces in finish.Expects the piece positions sorted in ascending order.
	*start - startIndex of pieces in finish in array pieces
	*end - endIndex of pieces in finish in array pieces
	*@return the id of the combination of pieces in finish
	*/
	int encodeFinishCombination(int start, int end);

	/**
	 * Encodes the combination of pieces in Game for the player to move. Expects the piece positions sorted in ascending order.
	 * @param start - startIndex of pieces in Game in array pieces
	 * @param end - endIndex of pieces in game in array pieces
	 * @return the id of the combination of pieces in Game
	 */
	uint64_t encodeInGameCombinationFirst(int start, int end);

	/**
	 * Encodes the combination of pieces in Game for the player not to move. Expects the piece positions sorted in ascending order.
	 * @param start - startIndex of pieces in Game in array pieces
	 * @param end - endIndex of pieces in game in array pieces
	 * @return the id of the combination of pieces in Game
	 */
	uint64_t encodeInGameCombinationSecond(int start, int end, vector<int> firstPositions, int startFirst, int endFirst);
};

#endif
