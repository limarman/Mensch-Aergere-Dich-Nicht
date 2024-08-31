#include "RatioAnalyser.h"
#include <math.h>
#include <map>
#include "IOResults.h"
#include <fstream>
#include <iostream>
#include "Calculator.h"
#include "MadN.h"
#include "Position.h"


int gcd(int a, int b)
{
	if (b == 0)
		return a;
	else
		return gcd(b, a % b);
}

double getRatio(int g_player1, int g_player2)
{
	int nominator, denominator;

	if (g_player1 <= g_player2) 
	{
		denominator = g_player2;
		nominator = g_player2 - g_player1;
	}
	else 
	{
		denominator = g_player1;
		nominator = g_player2 - g_player1;
	}

	//find out the greatest common divisor
	int common = gcd(fabs(nominator), denominator);

	nominator /= common;
	denominator /= common;

	return (static_cast<double>(nominator) / denominator);
}

void analyseRatios() 
{
	struct data
	{
		uint64_t number = 0; //number of probability entries
		double sum = 0; //sum of entries - using double for more precision
		float max = -1; //biggest entry (init <0)
		float min = 2; //smallest entry (init >1)

	};

	map<double, data> group;
	//pair<map<float, data>::iterator, bool> it;

	Calculator c(2, true); //loading probabilties from the result array

	for (uint64_t i = 0; i < numberOfPositions; i++) 
	{
		Position p = Position::decodePosition(i);
		double ratio = p.getPieceRatio();
		float probability = c.getProbability(i);

		//if (fabs(ratio - 0.97826087f) <= 0.00000001f)
		//{
		//	cout << i << "\n";
		//}

		//insert element
		group[ratio].sum += probability;
		group[ratio].number++;
		if (probability > group[ratio].max)
		{
			group[ratio].max = probability;
		}
		if (probability < group[ratio].min)
		{
			group[ratio].min = probability;
		}

		////insert new element if not yet inserted
		//it = group.insert(pair<float, data>(ratio, {1,probability,probability,probability}));

		////if already inserted, modify the struct
		//if (it.second == false) 
		//{
		//	group[ratio].sum += probability;
		//	group[ratio].number++;
		//	if (probability > group[ratio].max) 
		//	{
		//		group[ratio].max = probability;
		//	}
		//	if (probability < group[ratio].min)
		//	{
		//		group[ratio].min = probability;
		//	}
		//}
	}

	cout << "done with grouping\n";

	ofstream dataFile;
	dataFile.open("ratiodata/RatioData.csv");

	for (const auto& m : group)
	{
		dataFile.precision(8);
		dataFile << m.first << "," << m.second.number <<  "," << m.second.min << ","
			<< m.second.max << "," << m.second.sum / m.second.number << "\n";
	}

	dataFile.close();

	ofstream averageFile;
	averageFile.open("ratiodata/RatioDataAvg.csv");
	
	//averaging probabilites and ratios over multiple observations
	uint64_t numberOfObservations = 0;
	double overallSumProbs = 0;
	double overallSumRatio = 0;
	for (const auto& m : group) 
	{
		if (numberOfObservations >= 20) 
		{
			averageFile.precision(8);
			averageFile << overallSumRatio/numberOfObservations << "," << numberOfObservations
				<< "," <<overallSumProbs/numberOfObservations << "\n";

			numberOfObservations = 0;
			overallSumProbs = 0;
			overallSumRatio = 0;
		}
		numberOfObservations += m.second.number;
		overallSumProbs += m.second.sum;
		overallSumRatio += m.first * m.second.number; //weighting ratio accordingly
	}

	// save last chunk as well
	if (numberOfObservations != 0) 
	{
		averageFile.precision(8);
		averageFile << overallSumRatio / numberOfObservations << "," << numberOfObservations
			<< "," << overallSumProbs / numberOfObservations << "\n";
	}
	averageFile.close();

	//multimap<float, float> group;

	////partitioning all probabilities into ratio containers
	//for (uint64_t i = 0; i < numberOfPositions; i++) 
	//{
	//	Position p = Position::decodePosition(i);
	//	float ratio = p.getPieceRatio();
	//	float probability = readProbability(i);

	//	group.insert(pair<float, float>(ratio, probability));
	//}


	////iterating over all ratios and culling relevant data from probabilites
	//pair <multimap<float, float>::iterator, multimap<float, float>::iterator> ret;
	//ret = group.equal_range(ch);
}
