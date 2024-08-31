#include "Compressor.h"
#include "Position.h"
#include <iostream>


/*
This method generates the optimal move bit sequence for the given position ID and
the given vector of winning probabiltites.
*/
optimalMoveBitSequence getOptimalMoveBitSequence(uint64_t posID, vector<float>& probabilities) 
{

	Position pos = Position::decodePosition(posID);

	optimalMoveBitSequence res = 0;

	//if terminal position it does not make any sense to talk about the best succ
	if (pos.isFinished()) 
	{
		return res;
	}

	//fill the optimal-move-bit-sequence by looking at the different possible die rolls.
	for (int roll = 1; roll <= 6; roll++) 
	{		
		//uses that the pieces of the successor are not sorted again.
		vector<Position> succs = pos.calculateSuccessors(roll);
		
		float maxProb = 0;
		int bestSuccIndex = 0;

		//find the best successor in the current dieroll
		for (int index = 0; index < succs.size(); index++) 
		{
			Position succ = succs[index]; //copy as encoding has side-effects
			uint64_t succID = succ.encodePosition();
			//changing view on the probabilites
			float prob = (roll == 6 ? probabilities[succID] : 1 - probabilities[succID]);

			if (prob > maxProb) 
			{
				maxProb = prob;
				bestSuccIndex = index;
			}
		}

		int changedPiecePos = 0;

		//find the index of the piece which has changed - if none has changed then 11 = 3
		for (changedPiecePos = 0; changedPiecePos < pieceNumber; changedPiecePos++) 
		{
			//check whether the current index has changed (there can only be one change per move)
			//assumes that relative order of pieces has not been changed in the array (no sorting etc.)
			if (pos.getPlayer(0)->getPositionAtIndex(changedPiecePos) !=
				succs[bestSuccIndex].getPlayer(0)->getPositionAtIndex(changedPiecePos))
			{
				break;
			}
		}
		//There was no possible move
		if(changedPiecePos == pieceNumber)
		{
			changedPiecePos--;
		}

		//enter the index into the sequence
		res <<= 2;
		res += changedPiecePos;

	}

	return res;
}

optimalMoveUnit createOptimalMoveUnit(optimalMoveBitSequence sequence1,
	optimalMoveBitSequence sequence2)
{
	optimalMoveUnit unit;
	//create the lower (LSB) third of the unit
	unit.sequence2 = static_cast<char>(sequence2);

	//create the middle third of the unit
	unit.sequence12 = static_cast<char>(sequence1);
	unit.sequence12 <<= 4; //the upper four bits are now correct
	sequence2 >>= 8;
	unit.sequence12 |= sequence2;

	//create the upper (MSB) third of the unit
	sequence1 >>= 4;
	unit.sequence1 = static_cast<char>(sequence1);

	return unit;
}

void decodeOptimalMoveUnit(optimalMoveUnit unit, optimalMoveBitSequence* into) 
{
	optimalMoveBitSequence sequence1 = 0, sequence2 = 0;

	//bulding the second sequence
	sequence2 |= unit.sequence12;
	sequence2 <<= 8;
	sequence2 &= 0b111111111111; // delete all bits which are not the first 12
	//without bitwise 'and', the char is first casted to int by adding leading 1's
	sequence2 |= (0b11111111 & unit.sequence2);

	//building the first sequence
	sequence1 |= unit.sequence1;
	sequence1 <<= 4;
	sequence1 &= 0b111111111111; // delete all bits which are not the first 12
	unit.sequence12 >>= 4; //when it is negative it creates 1's in the beginning so we delete optional 1's at the end
	unit.sequence12 &= 0b00001111;
	sequence1 |= unit.sequence12;

	//writing to memory
	into[0] = sequence1; 
	into[1] = sequence2;

}


int getOptimalMovePieceIndex(optimalMoveBitSequence sequence, int roll)
{
	for (int i = 0; i < 6 - roll; i++) 
	{
		sequence >>= 2; //shift the important indexes to the beginning
	}

	//erase all other bits
	sequence &= 3;

	return sequence;
}


