#include "MadN.h"
#include <iostream>
#include "Player.h"
#include "Position.h"
#include "Calculator.h"
#include "IOResults.h"
#include "RatioAnalyser.h"
#include <thread>
#include <fstream>

//private Methods
uint64_t calculateNumberOfPositions();

void initializeIDLookUPTable();
void initializePossibilities();
void initializePieceDistributionIDs();
void initializeBaseNumber();

//Attributes
vector<uint64_t> configurationBases;
vector<AbstractPosition> distributionIDs;
vector<Possibility> possibilities;

int pieceNumber;
int squareNumber;
int baseNumber;
uint64_t numberOfPositions;


void initialize(int pieces, int squares)
{
	//initialize everything in MadN.h
	//numberOfPositions = calculateNumberOfPositions(); 
	//TODO: problems with making number of positions, (number of pieces, squares) variable.
	//Need project restructuring

	pieceNumber = pieces;
	squareNumber = squares;
	numberOfPositions = calculateNumberOfPositions();
	initializeBaseNumber();

	configurationBases.resize(baseNumber);
	distributionIDs.resize(baseNumber);
	possibilities.resize(baseNumber);

	initializePieceDistributionIDs();
	initializePossibilities();
	initializeIDLookUPTable();
}

/**
* Needs DistributionsIDs to be initialized
*/
void initializeIDLookUPTable()
{

	//initializePieceDistributionIDs();
	vector<uint64_t> idLookUpTableN(baseNumber);

	//calculate number of possibilities for every distribution case
	for (int i = 0; i < baseNumber; i++)
	{
		AbstractPosition ap = distributionIDs[i];

		idLookUpTableN[i] = possibilitiesConfiguration(&ap);
	}

	for (int i = 1; i < baseNumber; i++)
	{
		configurationBases[i] = idLookUpTableN[i - 1] + configurationBases[i - 1];
	}

}

/**
* needs distribution IDs to be initialized
*/
void initializePossibilities()
{

	for (int i = 0; i < baseNumber; i++)
	{
		//possibilities[i] = new Possibility();
		AbstractPosition ap = distributionIDs[i];
		uint64_t configPossibilities = possibilitiesConfiguration(&ap);

		//after setting the finished pieces of first player
		configPossibilities /= binomialCoefficient(pieceNumber, ap.inFinish[0]);
		possibilities[i].setFinishFirst = configPossibilities;

		//after setting the finished pieces of second player
		configPossibilities /= binomialCoefficient(pieceNumber, ap.inFinish[1]);
		possibilities[i].setFinishSecond = configPossibilities;

		//after setting the in Game pieces of first player
		configPossibilities /= binomialCoefficient(squareNumber, ap.inGame[0] - ap.inFinish[0]);
		possibilities[i].setGameFirst = configPossibilities;
	}
}

void initializePieceDistributionIDs()
{
	int base = (pieceNumber + 1) * (pieceNumber + 2) / 2;

	for (int i = 0; i < baseNumber; i++)
	{
		//decomposing total ID into the IDs for each player
		int aID = i / base;
		int bID = i % base;

		//decoding ID of players into abstract position

		//First the player to move
		int inGameA = 0, inFinishA = 0;

		for (int j = pieceNumber; j >= 0; j--)
		{
			int value = j * (j + 1) / 2;
			if (value <= aID)
			{
				inGameA = j;
				inFinishA = aID - value;
				break;
			}
		}

		//Then second player player to move
		int inGameB = 0, inFinishB = 0;

		for (int j = pieceNumber; j >= 0; j--)
		{
			int value = j * (j + 1) / 2;
			if (value <= bID)
			{
				inGameB = j;
				inFinishB = bID - value;
				break;
			}
		}

		distributionIDs[i] = AbstractPosition{ {inGameA, inGameB}, {inFinishA, inFinishB} };

	}
}

void initializeBaseNumber()
{
	baseNumber = (pieceNumber + 1) * (pieceNumber + 1) * (pieceNumber + 2) * (pieceNumber + 2) / 4 - 1;
}

uint64_t possibilitiesConfiguration(AbstractPosition* ap)
{
	return binomialCoefficient(squareNumber, ap->inGame[0] - ap->inFinish[0]) * binomialCoefficient(pieceNumber, ap->inFinish[0]) *
		binomialCoefficient(squareNumber - (ap->inGame[0] - ap->inFinish[0]), ap->inGame[1] - ap->inFinish[1]) * binomialCoefficient(pieceNumber, ap->inFinish[1]);
}

uint64_t calculateNumberOfPositions() 
{
	uint64_t res = 0;
	for (int i = 0; i <= pieceNumber; i++)
	{
		for (int j = 0; j <= i; j++)
		{
			for (int i2 = 0; i2 <= pieceNumber; i2++)
			{
				for (int j2 = 0; j2 <= i2; j2++)
				{
					res += binomialCoefficient(squareNumber, i - j) * binomialCoefficient(pieceNumber, j) *
						binomialCoefficient(squareNumber - (i - j), i2 - j2) * binomialCoefficient(pieceNumber, j2);
				}
			}
		}
	}

	//both finish is not possible
	return res - 1;
}

uint64_t binomialCoefficient(int n, int k)
{
	if (k == 0)
	{
		return 1;
	}

	uint64_t result = 1;

	for (uint64_t i = 1; i <= k; i++)
	{
		result = result * (n - i + 1) / i;
	}

	return result;
}

void checkBijectionFromTo(uint64_t from, uint64_t to) 
{
	for (uint64_t i = from; i <= to; i++)
	{

		if (Position::decodePosition(i).encodePosition() != i)
		{
			std::cout << "Fehler bei ID: " << i << "\n";
			break;
		}
	}
}

void checkBijection() 
{
	const int numberOfThreads = 2;
	uint64_t ranges[numberOfThreads]; //start index for every thread
	uint64_t positionsPerThread = numberOfPositions / numberOfThreads;

	for (int i = 0; i < numberOfThreads; i++)
	{
		ranges[i] = i * positionsPerThread;
	}

	vector<thread> threads;

	//Creating the Threads
	for (int i = 0; i < numberOfThreads - 1; i++)
	{
		//threads[i] = thread t(iterateFromTo, ranges[i], ranges[i + 1] - 1, numberOfChanges);
		threads.emplace_back(&checkBijectionFromTo, ranges[i], ranges[i + 1] - 1);
		//threads.emplace_back([this, &]() {this->iterateFromTo(ranges[i], ranges[i + 1] - 1, numberOfChanges);});

	}
	threads.emplace_back(&checkBijectionFromTo,
		ranges[numberOfThreads - 1], numberOfPositions - 1);

	for (auto& t : threads)
	{
		t.join();
	}

	cout << "Bijektion Test ended. If no errors then passed!\n";
}



