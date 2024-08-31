/*
Class for compression of the output file, by condensing the winning probabiltites and only saving the
best moves for all different die rolls.
Because there are 2 bits used per dieroll, we can only encode up to 4 different possibilites. Thus
this encoding works only for 4 or less pieces and in particular the move "no move" (if one has to pass the turn)
is encoded by (11)_2 which is equivalent to "move the furthest piece".
This has to be taken care of when using the compressed best move file as foundation for game simulation. 
*/

#include "MadN.h"

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

/*
Only the first 12 Bits are relevant. Consists of 6*2 bits indexing the best move for every die roll.
*/
#define optimalMoveBitSequence uint32_t

///*
//This struct is a bitfield which saves 6 blocks of 2 bits. For every different die roll (6)
//there is a maximum of 4 possible moves (as there are only 4 different pieces).
//These 4 different moves are encoded with 2 bits. Thus 6*2 = 12 bits. The two most significant bits
//are for the die roll 1.
//*/
//struct optimalMoveBitSequence 
//{
//	unsigned int sequence : 12;
//};

/*
This struct is a union of two optimal move bit sequences to get on a bitcount which is divisible by 8
(seperable in Bytes) for efficient writing to the hard drive. Thereby we use char, as otherwise the
struct is paddded to 4 bytes.
*/
struct optimalMoveUnit 
{
	char sequence1, sequence12, sequence2;
};

/*
This method generates the optimal move bit sequence for the given position ID and
the given vector of winning probabiltites.
*/
optimalMoveBitSequence getOptimalMoveBitSequence(uint64_t posID, vector<float>& probabilities);


/*
This method is given a binary sequence of encoded optimal moves (see optimalMoveBitSequence) and
the die roll.
It extracts the information in the sequence at the position responsible for the die roll and returns
the index of the piece which would move when applying the best move.
(Assuming the positions of the pieces are sorted.)
*/
int getOptimalMovePieceIndex(optimalMoveBitSequence sequence, int roll);

/*
creates the unit of 2 optimalMoveSequences (see optimalMoveUnit)
*/
optimalMoveUnit createOptimalMoveUnit(optimalMoveBitSequence sequence1,
	optimalMoveBitSequence sequence2);

/*
Decodes the optimalMoveUnit unit into the corresponding two optimalMoveBitSequences.
Pointer into should have enough space for two optimalMoveBitSequences
*/
void decodeOptimalMoveUnit(optimalMoveUnit unit, optimalMoveBitSequence* into);

#endif

