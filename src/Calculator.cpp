#include "Calculator.h"
#include "Position.h"
#include <thread>
#include <functional>
#include <cmath> //necessary for g++?
#include <iostream>
#include "IOResults.h"

Calculator::Calculator(int numberOfThreads, bool load) : probabilities(numberOfPositions)
{
	this->numberOfThreads = numberOfThreads;
	stringstream stream;

	stream.str(string());
	stream << "Initialization of Calculator started:\n";
	logCout(stream);
	//create neessary directories for saving files
	//experimental::filesystem::create_directory(backUpDirectory);
	//experimental::filesystem::create_directory(logDirectory);
	//experimental::filesystem::create_directory(resultDirectory);

	if (!load) { //initialize with all threads

		stream.str(string());
		stream << "-- Start values are estimated. (Not loaded)\n";
		logCout(stream);

		vector<uint64_t> ranges(numberOfThreads); //start index for every thread
		uint64_t positionsPerThread = numberOfPositions / numberOfThreads;

		for (int i = 0; i < numberOfThreads; i++)
		{
			ranges[i] = i * positionsPerThread;
		}

		vector<thread> threads;

		//Creating the Threads
		for (int i = 0; i < numberOfThreads - 1; i++)
		{
			threads.emplace_back(&Calculator::initFromTo, this, ranges[i], ranges[i + 1] - 1, 0.5f);

		}
		threads.emplace_back(&Calculator::initFromTo, this,
			ranges[numberOfThreads - 1], numberOfPositions - 1, 0.5f);

		for (auto& t : threads)
		{
			t.join();
		}

	}
	else 
	{
		string loadedFile = "backup/backupA.bin";
		stream.str(string());
		stream << "-- Start values are loaded from " << loadedFile << "\n";
		logCout(stream);

		//load from file
		loadProbabilities(probabilities, loadedFile);
	}
}

Calculator::Calculator(int numberOfThreads, string loadFile) : probabilities(numberOfPositions)
{
	this->numberOfThreads = numberOfThreads;

	stringstream stream;

	stream.str(string());
	stream << "Initialization of Calculator started:\n";
	logCout(stream);
	//create neessary directories for saving files
	//experimental::filesystem::create_directory(backUpDirectory);
	//experimental::filesystem::create_directory(logDirectory);
	//experimental::filesystem::create_directory(resultDirectory);

		stream.str(string());
		stream << "-- Start values are loaded from " << loadFile << "\n";
		logCout(stream);

		//load from file
		loadProbabilities(probabilities, loadFile);

}

void Calculator::calculateProbabilitiesParallel()
{
	atomic<uint64_t> numberOfChanges{1}; //not 0  and using "= 1" causes problems in g++ compiler
	int iteration = 0;
	bool backUpFile = true; //decides which file should be written in next
	stringstream stream;

	stream.str(string());
	stream << "Started calculation:\n";
	stream << "-- Number of pieces: " << pieceNumber << "\n";
	stream << "-- Number of squares: " << squareNumber << "\n";
	stream << "-- Number of calculating threads: " << numberOfThreads << "\n";
	logCout(stream);

	vector<uint64_t> ranges(numberOfThreads); //start index for every thread
	uint64_t positionsPerThread = numberOfPositions / numberOfThreads;

	for (int i = 0; i < numberOfThreads; i++) 
	{
		ranges[i] = i * positionsPerThread;
	}


	//iterate until there is no change left
	while (numberOfChanges != 0)
	{
		iteration++;
		stream.str(string()); //clearing the stream
		stream << "Number of changed entries: " << numberOfChanges << "\n";
		stream << "Probability of start position: " << probabilities[0] << "\n";
		stream << "Iteration: " << iteration << "\n";
		logCout(stream); //writing to cout and log
		numberOfChanges = 0;

		vector<thread> threads;

		//Creating the Threads
		for (int i = 0; i < numberOfThreads-1; i++) 
		{
			threads.emplace_back(&Calculator::iterateFromTo, this, ranges[i], ranges[i + 1] - 1, ref(numberOfChanges));
		}
		threads.emplace_back(&Calculator::iterateFromTo, this,
			ranges[numberOfThreads - 1], numberOfPositions - 1, ref(numberOfChanges));

		for (auto& t : threads)
		{
			t.join();
		}
		

		//backing up data
		//if (iteration % 5 == 0) 
		//{
		//	if (backUpFile) 
		//	{
		//		stream.str(string());
		//		stream << "Backing up data to " << backUpFileA << "\n";
		//		logCout(stream);
		//		backupProbabilites(probabilities, backUpFileA);
		//	}
		//	else 
		//	{
		//		stream.str(string());
		//		stream << "Backing up data to " << backUpFileB << "\n";
		//		logCout(stream);
		//		backupProbabilites(probabilities, backUpFileB);
		//	}
		//	//backUpFile = !backUpFile; //toggle for next time
		//}
		
	}

	//save results
	stream.str(string()); //clearing the stream
	stream << "Saving results... \n";
	logCout(stream);

	saveProbabilities(probabilities, true, resultFileBinary);
	//saveProbabilities(probabilities, false, resultFileText);
}

float Calculator::calculateExpectancyStep(uint64_t positionID)
{

	Position p = Position::decodePosition(positionID);

	//check all possible dice rolls
	float prob = 0;
	float maxVal = 1; //the worst winning probability for opponent for the current diceroll
	for (int roll = 1; roll < 6; roll++)
	{
		for (Position succ : p.calculateSuccessors(roll))
		{
			uint64_t encoded = succ.encodePosition();

			//instead of maximizing 1-prob: it is more stable to minimize prob
			if (probabilities[encoded] < maxVal)
			{
				maxVal = probabilities[encoded];
			}

		}
		prob += maxVal;
		maxVal = 1;
		//	System.out.println("maxVal:" + maxVal);
	}

	//for 6 special case as turn does not change
	maxVal = 0; //the best winning probability in the current diceroll
	for (Position succ : p.calculateSuccessors(6))
	{
		uint64_t encoded = succ.encodePosition();

		if (probabilities[encoded] > maxVal)
		{
			maxVal = probabilities[encoded];
		}
	}
	//more or less stable calculation
	return (5 + maxVal - prob) / 6;

}

float Calculator::getProbability(uint64_t positionID)
{
	return probabilities[positionID];
}

void Calculator::getSelectivity(float* minSelec)
{
	vector<uint64_t> ranges(numberOfThreads); //start index for every thread
	vector<float> minSelecPerThread(numberOfThreads); //minimum for every single thread

	uint64_t positionsPerThread = numberOfPositions / numberOfThreads;

	for (int i = 0; i < numberOfThreads; i++)
	{
		ranges[i] = i * positionsPerThread;
	}

	vector<thread> threads;

	//Creating the Threads
	for (int i = 0; i < numberOfThreads - 1; i++)
	{
		threads.emplace_back(&Calculator::getSelectivityFromTo, this, ranges[i], ranges[i + 1] - 1, ref(minSelecPerThread[i]));
	}
	threads.emplace_back(&Calculator::getSelectivityFromTo, this,
		ranges[numberOfThreads - 1], numberOfPositions - 1, ref(minSelecPerThread[numberOfThreads-1]));

	for (auto& t : threads)
	{
		t.join();
	}

	//find the minimum of all threads
	float minSelectivity = 1;
	for (float minpT : minSelecPerThread) 
	{
		if (minpT < minSelectivity) 
		{
			minSelectivity = minpT;
		}
	}

	*minSelec = minSelectivity;
}

uint64_t Calculator::getMinWinningProbabilityPositionID()
{
	float min = 1;
	uint64_t minPos  = 0;
	for (int i = 0; i < numberOfPositions; i++) 
	{
		if (probabilities[i] != 0 && probabilities[i] < min) 
		{
			minPos = i;
			min = probabilities[i];
		}
	}

	return minPos;
}

uint64_t Calculator::getMaxWinningProbabilityPositionID()
{
	float max = 0;
	uint64_t maxPos = 0;
	for (int i = 0; i < numberOfPositions; i++)
	{
		if (probabilities[i] != 1 && probabilities[i] > max)
		{
			maxPos = i;
			max = probabilities[i];
		}
	}
	return maxPos;
}

void Calculator::outputOptimalMoves()
{
	saveBestMoves(probabilities, false);
}

void Calculator::iterateFromTo(uint64_t from, uint64_t to, atomic<uint64_t>& changes)
{
	uint64_t numberOfChanges = 0;

	//iterate through all entrys
	for (uint64_t i = from; i <= to; i++)
	{
		//do not change the definite results
		if (probabilities[i] == 1 || probabilities[i] == 0)
		{
			continue;
		}

		//calculate
		float prob = calculateExpectancyStep(i);

		//does not allow position to fall out of scope
		if (prob == 1)
		{
			prob = 0.9999999f;
		}

		//relaxed equality for convergence
		if (!consideredEqual(probabilities[i], prob))
		{
			numberOfChanges++;
		}


		//update probability
		probabilities[i] = prob;
	}

	//collecting changes in reference
	changes += numberOfChanges;
}


void Calculator::initFromTo(uint64_t from, uint64_t to, float with) 
{
	for (uint64_t i = from; i <= to; i++)
	{
		Position p = Position::decodePosition(i);
		if (p.getPlayer(0)->hasFinished())
		{
			probabilities[i] = 1;
		}
		else if (p.getPlayer(1)->hasFinished())
		{
			probabilities[i] = 0;
		}
		else
		{
			probabilities[i] = with;
		}
	}
}

void Calculator::initFromToEstimate(uint64_t from, uint64_t to) 
{
	//polynom coefficients in descending order (of exponent) - regression polynom from data of 3-stone instance
	double coefficients[] = { -0.373, 1.95, 0.452, -2.58, -0.0918, 1.22, -7.23e-4, -0.35, -0.0116, 0.255, 0.525 };
	//degree of polynom - number of coefficients-1
	int degree = 10;

	for (uint64_t i = from; i <= to; i++) 
	{
		Position p = Position::decodePosition(i);
		double ratio = p.getPieceRatio();

		if (ratio == 1) 
		{
			//equivalent to a won position
			probabilities[i] = 1;
		}
		else if (ratio == -1) 
		{
			//equivalent to a lost position
			probabilities[i] = 0;
		}
		else 
		{
			//evaluate regression polynom to find out estimate value - Horner
			double value = coefficients[0];

			for (int p = 1; p <= degree; p++) 
			{
				value = value * ratio + coefficients[p];
			}

			probabilities[i] = static_cast<float>(value);
		}

	}


}

bool Calculator::consideredEqual(float f1, float f2) 
{
	return (fabs(f1 - f2) <= 0.000001f);
}

void Calculator::getSelectivityFromTo(uint64_t from, uint64_t to, float& selectivity)
{
	float minimalSelectivity = 1;

	for (uint64_t i = from; i <= to; i++) 
	{
		Position p = Position::decodePosition(i);
		float maxVal = -1, secondMaxVal = -1;

		if (p.isFinished()) 
		{
			continue;
		}

		for (int roll = 1; roll < 6; roll++)
		{
			for (Position succ : p.calculateSuccessors(roll))
			{
				uint64_t encoded = succ.encodePosition();

				//finding out maximum and minimum
				if (probabilities[encoded] > maxVal)
				{
					secondMaxVal = maxVal;
					maxVal = probabilities[encoded];
				}
				else if (probabilities[encoded] > secondMaxVal) 
				{
					secondMaxVal = probabilities[encoded];
				}

			}

			//update selectivity
			float selec = maxVal - secondMaxVal; 
			if (selec > 0) { //sort out complete indifferent choices
				if (selec < minimalSelectivity)
				{
					minimalSelectivity = selec;
				}
			}
			maxVal = -1, secondMaxVal = -1;
		}

		//for 6 special case as turn does not change
		for (Position succ : p.calculateSuccessors(6))
		{
			uint64_t encoded = succ.encodePosition();

			if (probabilities[encoded] > maxVal)
			{
				secondMaxVal = maxVal;
				maxVal = probabilities[encoded];
			}
			else if (probabilities[encoded] > secondMaxVal)
			{
				secondMaxVal = probabilities[encoded];
			}
		}

		//update selectivity
		float selec = maxVal - secondMaxVal;
		if (selec > 0) { //sort out complete indiferrent choices
			if (maxVal - secondMaxVal < minimalSelectivity)
			{
				minimalSelectivity = maxVal - secondMaxVal;
			}
		}
	}

	selectivity = minimalSelectivity;
}

void Calculator::getAverageMistakeFromTo(uint64_t from, uint64_t to, float& avgMistake)
{
	return;
}



void Calculator::outputResults() 
{
	saveProbabilities(probabilities, false, resultFileTxt);
}

float Calculator::getAverageMistake(GamePlayer* gp)
{
	return 0.0f;
}
