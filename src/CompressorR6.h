/*
Class for compression of the output file, by condensing the winning probabiltites and only saving the
best moves. 

Because there are 2 bits used per dieroll, we can only encode up to 4 different possibilites. Thus
this encoding works only for 4 or less pieces and in particular the move "no move" (if one has to pass the turn)
is encoded by (11)_2 which is equivalent to "move the furthest piece". This has to be taken care of when
using the compressed best move file as foundation for game simulation.

This compressor (other than Compressor) is designed to group best moves to their respective die roll
(therefore the naming scheme _r). Meaning that it creates a binary representation of best moves only considering
one specific die roll.
*/

#include "MadN.h"

#ifndef COMPRESSORR6_H
#define COMPRESSORR6_H

	/*
	Consists of two bits indexing the best move for one specific die roll. Only the first two (LSB)
	bits are relevant.
	*/
	#define optimalMoveBits_r unsigned char

	/*
	Consists of a unit of 4 optimalMoveBits for 4 different positions all of them for the same dieroll, reaching exactly one byte (size of char)
	*/
	#define optimalMoveUnit4_r unsigned char

class CompressorR6
{

private:

	int numberOfThreads;

	void calculateBestMovesFromTo(vector<float>* probabilities, vector<optimalMoveUnit4_r>* into[6],
		uint64_t from, uint64_t to);

	void calculateBestMovesFromTo_r(vector<float>* probabilities, vector<optimalMoveUnit4_r>* into, int dieroll,
		uint64_t from, uint64_t to);


public:

	/*
	Initializes CompressorR6 class
	"numberOfThreads" is the number of threads the compressor will use to compress the probability file (parallelization)
	*/
	CompressorR6(int numberOfThreads);

	/*
	This method generates the optimal movebits (see optimalMoveBits) for the given position ID,
	the given vector of winning probabiltites and the given dieroll.
	*/
	static optimalMoveBits_r getOptimalMoveBits_r(uint64_t posID, vector<float>& probabilities, int dieroll);


	/*
	creates the unit of 4 optimalMoveBits (see optimalMoveUnit4)
	*/
	static optimalMoveUnit4_r createOptimalMoveUnit4_r(optimalMoveBits_r bits[4]);


	/*
	Decodes the optimalMoveUnit4 unit into the corresponding four optimalMoveBits.
	Pointer into should have enough space for four optimalMoveBits (4 Byte)
	*/
	static void decodeOptimalMoveUnit4_r(optimalMoveUnit4_r unit, optimalMoveBits_r* into);

	/*
	Filling the array "into" with optimalMoveUnits for every position ID and given "dieroll".
	As every unit consists of 4 positions, the vector "into" ís resized to size ceil(numberOfPositions / 4) 
	optimal move units
	*/
	void calculateBestMoves_r(vector<float>& probabilities, vector<optimalMoveUnit4_r>& into, int dieroll);

	/*
	Filling the six arrays "into" with optimalMoveUnits for every position ID and for given "dieroll" respectively.
	As every unit consists of 4 positions, the vector "into" ís resized to size ceil(numberOfPositions / 4)
	optimal move units
	*/
	void calculateBestMoves(vector<float>& probabilities, vector<optimalMoveUnit4_r>* into[6]);

};

#endif
