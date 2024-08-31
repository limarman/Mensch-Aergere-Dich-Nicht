#include "CompressorR6.h"
#include "Position.h"
#include <thread>
#include <cmath>


CompressorR6::CompressorR6(int numberOfThreads) 
{
	this->numberOfThreads = numberOfThreads;
}

optimalMoveBits_r CompressorR6::getOptimalMoveBits_r(uint64_t posID, vector<float>& probabilities, int dieroll)
{
	Position pos = Position::decodePosition(posID);

	optimalMoveBits_r res = 0;

	//if terminal position it does not make any sense to talk about the best succ
	if (pos.isFinished())
	{
		return res;
	}

	//uses that the pieces of the successor are not sorted again.
	vector<Position> succs = pos.calculateSuccessors(dieroll);

	float maxProb = 0;
	int bestSuccIndex = 0;

	//find the best successor in the current dieroll
	for (int index = 0; index < succs.size(); index++)
	{
		Position succ = succs[index]; //copy as encoding has side-effects
		uint64_t succID = succ.encodePosition();
		//changing view on the probabilites
		float prob = (dieroll == 6 ? probabilities[succID] : 1 - probabilities[succID]);

		if (prob > maxProb)
		{
			maxProb = prob;
			bestSuccIndex = index;
		}
	}

	int changedPiecePos = 0;

	//find the index of the piece which has changed - if none has changed then (11)_2 = 3
	//(or piecenumber-1 in general case)
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
	if (changedPiecePos == pieceNumber)
	{
		changedPiecePos--;
	}

	res += changedPiecePos;


	return res;
}


optimalMoveUnit4_r CompressorR6::createOptimalMoveUnit4_r(optimalMoveBits_r bits[4])
{
	optimalMoveUnit4_r unit = 0;
	
	//merging all the bits into one char
	for (int i = 0; i < 4; i++) 
	{
		unit <<= 2;
		unit |= bits[i];
	}

	return unit;
}

void CompressorR6::decodeOptimalMoveUnit4_r(optimalMoveUnit4_r unit, optimalMoveBits_r* into)
{
	//filling in reversed order, pulling the stack of optimalMove bits 
	//which has been pushed during creation of unit
	for (int i = 3; i >= 0; i--) 
	{
		into[i] = unit & 0b00000011; //masking out the two least significant bit
		unit >>= 2;
	}
}

void CompressorR6::calculateBestMoves_r(vector<float>& probabilities, vector<optimalMoveUnit4_r>& into, int dieroll)
{
	//number of optimalMoveUnits that have to be created
	uint64_t numberOfUnits = ceil(numberOfPositions / 4.0);

	//resizing optimalMoveUnit vector to appropriate size
	into.resize(numberOfUnits);


	//vector<optimalMoveUnit4_r> into2;
	//vector<float> probabilities2;

	//splitting the job into numberOfThreads roughly equal sized jobs
	//start index of move unit, thread i has to work on
	vector<uint64_t> ranges(numberOfThreads); 
	uint64_t unitsPerThread = numberOfUnits / numberOfThreads; //except for the last thread, which might get more 

	for (int i = 0; i < numberOfThreads; i++)
	{
		ranges[i] = i * unitsPerThread;
	}

	vector<thread> threads;

	//threads.emplace_back(&CompressorR6::dummy, this, 
	//	&probabilities);

	//Creating the Threads
	for (int i = 0; i < numberOfThreads - 1; i++)
	{
		threads.emplace_back(&CompressorR6::calculateBestMovesFromTo_r, this,
			&probabilities, &into, dieroll, ranges[i], ranges[i + 1] - 1);
	}
	threads.emplace_back(&CompressorR6::calculateBestMovesFromTo_r, this,
		&probabilities, &into, dieroll, ranges[numberOfThreads-1], numberOfUnits - 1);

	for (auto& t : threads)
	{
		t.join();
	}
}

void CompressorR6::calculateBestMoves(vector<float>& probabilities, vector<optimalMoveUnit4_r>* into[6])
{
	//number of optimalMoveUnits that have to be created
	uint64_t numberOfUnits = ceil(numberOfPositions / 4.0);

	//resizing optimalMoveUnit vectors to appropriate size
	for (int i = 0; i < 6; i++) 
	{
		(*(into[i])).resize(numberOfUnits);
	}


	//vector<optimalMoveUnit4_r> into2;
	//vector<float> probabilities2;

	//splitting the job into numberOfThreads roughly equal sized jobs
	//start index of move unit, thread i has to work on
	vector<uint64_t> ranges(numberOfThreads);
	uint64_t unitsPerThread = numberOfUnits / numberOfThreads; //except for the last thread, which might get more 

	for (int i = 0; i < numberOfThreads; i++)
	{
		ranges[i] = i * unitsPerThread;
	}

	vector<thread> threads;

	//threads.emplace_back(&CompressorR6::dummy, this, 
	//	&probabilities);

	//Creating the Threads
	for (int i = 0; i < numberOfThreads - 1; i++)
	{
		threads.emplace_back(&CompressorR6::calculateBestMovesFromTo, this,
			&probabilities, into, ranges[i], ranges[i + 1] - 1);
	}
	threads.emplace_back(&CompressorR6::calculateBestMovesFromTo, this,
		&probabilities, into, ranges[numberOfThreads - 1], numberOfUnits - 1);

	for (auto& t : threads)
	{
		t.join();
	}
}

void CompressorR6::calculateBestMovesFromTo(vector<float>* probabilities, vector<optimalMoveUnit4_r>* into[6], uint64_t from, uint64_t to)
{
	//iterate through all units assigned to thís job
	for (uint64_t unitID = from; unitID <= to; unitID++)
	{
		//optimal move bits for the next four positions for every dieroll
		optimalMoveBits_r bits[6][4] = { 0 };

		uint64_t posID = unitID * 4; //changing between unit space and position space

		for (int i = 0; i < 4; i++)
		{
			for (int roll = 0; roll < 6; roll++) 
			{
				if (posID + i < numberOfPositions)
				{
					bits[roll][i] = getOptimalMoveBits_r(posID + i, *probabilities, roll+1);
				}
			}

			
		}

		for (int roll = 0; roll < 6; roll++) 
		{
			optimalMoveUnit4_r unit = createOptimalMoveUnit4_r(bits[roll]);
			(*into)[roll][unitID] = unit;
		}
		
	}
}

void CompressorR6::calculateBestMovesFromTo_r(vector<float>* probabilities, vector<optimalMoveUnit4_r>* into, int dieroll,
	uint64_t from, uint64_t to) 
{
	//iterate through all units assigned to thís job
	for (uint64_t unitID = from; unitID <= to; unitID++) 
	{
		//optimal move bits for the next for positions
		optimalMoveBits_r bits[4] = { 0 };

		uint64_t posID = unitID * 4; //changing between unit space and position space

		for (int i = 0; i < 4; i++)
		{
			if (posID + i < numberOfPositions)
			{
				bits[i] = getOptimalMoveBits_r(posID + i, *probabilities, dieroll);
			}
		}

		optimalMoveUnit4_r unit = createOptimalMoveUnit4_r(bits);
		(*into)[unitID] = unit;
	}
}
