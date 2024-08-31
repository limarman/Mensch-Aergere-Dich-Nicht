/*

*/

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "MadN.h"
#include <atomic>
#include <string>
#include "GamePlayer.h"

class Calculator
{
private:
	vector<float> probabilities;

	int numberOfThreads = 2;

	//backUpFiles
	const string backUpDirectory = "./backup";
	const string backUpFileA = backUpDirectory + "/backupA.bin";
	const string backUpFileB = backUpDirectory + "/backupB.bin";

	void iterateFromTo(uint64_t from, uint64_t to, atomic<uint64_t>& changes);

	/*
	Initializes all positions from ID "from" to ID to "to" (including "to")
	*/
	void initFromTo(uint64_t from, uint64_t to, float with);

	void initFromToEstimate(uint64_t from, uint64_t to);

	bool consideredEqual(float f1, float f2);

	void getSelectivityFromTo(uint64_t from, uint64_t to, float& selectivity);

	void getAverageMistakeFromTo(uint64_t from, uint64_t to, float& avgMistake);

public:
	//if loads then load start values from backup/backupA.bin
	Calculator(int numberOfThreads, bool load);

	//load start values from loadFile
	Calculator(int numberOfThreads, string loadFile);

	void calculateProbabilitiesParallel();

	float calculateExpectancyStep(uint64_t positionID);

	float getProbability(uint64_t positionID);

	/*
	Fills the parameters minSelec and avgSelec to get how close winning probabilitites get
	between possible successors.
	For selectivity between optimal moves.
	*/
	void getSelectivity(float* minSelec);

	uint64_t getMinWinningProbabilityPositionID();

	uint64_t getMaxWinningProbabilityPositionID();

	void outputOptimalMoves();

	void outputResults();

	float getAverageMistake(GamePlayer* gp);

};

#endif