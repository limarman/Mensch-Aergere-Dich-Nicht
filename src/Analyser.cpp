#include "Analyser.h"
#include "OptimalPlayer.h"
#include <thread>
#include "IOResults.h"

Analyser::Analyser(int numberOfThreads, vector<float>* probabilities)
{
	this->numberOfThreads = numberOfThreads;
	this->probabilities = probabilities;
}

similarityStats Analyser::compareToOptimal(GamePlayer* gp)
{
	OptimalPlayer op(probabilities);
	return compareGamePlayers(gp, &op);
}

similarityStats Analyser::compareGamePlayers(GamePlayer* gp1, GamePlayer* gp2)
{
	similarityStats output = { 0,0,0 };

	vector<uint64_t> ranges(numberOfThreads); //start index for every thread
	vector<similarityStats> outputPerThread(numberOfThreads); //minimum for every single thread

	uint64_t positionsPerThread = numberOfPositions / numberOfThreads;

	for (int i = 0; i < numberOfThreads; i++)
	{
		ranges[i] = i * positionsPerThread;
	}
	vector<thread> threads;

	//Creating the Threads
	for (int i = 0; i < numberOfThreads - 1; i++)
	{
		threads.emplace_back(&Analyser::compareGamePlayersFromTo, this, ranges[i], ranges[i + 1] - 1,
			gp1, gp2, &outputPerThread[i]);
	}
	threads.emplace_back(&Analyser::compareGamePlayersFromTo, this, ranges[numberOfThreads - 1], numberOfPositions - 1,
		gp1, gp2, &outputPerThread[numberOfThreads - 1]);

	for (auto& t : threads)
	{
		t.join();
	}

	for (similarityStats stats : outputPerThread) 
	{
		output.non_finished_situations += stats.non_finished_situations;
		output.non_trivial_situations += stats.non_trivial_situations;
		output.same_output_situations += stats.same_output_situations;
	}

	return output;
}

void Analyser::compareGamePlayersFromTo(uint64_t from, uint64_t to, GamePlayer* gp1, GamePlayer* gp2,
	similarityStats* stats)
{
	uint64_t same = 0;
	uint64_t non_trivial_situation = 0; //situation where there is more than 1 choice
	uint64_t non_finished_situation = 0; //situation where there are at least 1 choice

	for (uint64_t id = from; id <= to; id++)
	{
		//check whether the position is non-terminal
		Position p = Position::decodePosition(id);
		if (p.isFinished())
		{
			continue;
		}


		//if not terminal position
		for (int roll = 1; roll <= 6; roll++)
		{
			non_finished_situation++;

			//find the optimal successor based on the proability vector
			vector<Position> succs = p.calculateSuccessors(roll);

			if (succs.size() > 1)
			{
				non_trivial_situation++;
			}

			Position p1 = (*gp1).chooseSuccessor(p, roll);
			Position p2 = (*gp2).chooseSuccessor(p, roll);

			uint64_t succID1 = p1.encodePosition();
			uint64_t succID2 = p2.encodePosition();

			//check whether Prob and BM give the same successor by comparing their position IDs
			if (succID1 == succID2)
			{
				same++;
			}
		}
	}

	(*stats).non_finished_situations = non_finished_situation;
	(*stats).non_trivial_situations = non_trivial_situation;
	(*stats).same_output_situations = same;
}

void Analyser::idMapBijectionTest() 
{
	vector<uint64_t> ranges(numberOfThreads); //start index for every thread
	vector<bool> outputPerThread(numberOfThreads); //minimum for every single thread

	uint64_t positionsPerThread = numberOfPositions / numberOfThreads;

	for (int i = 0; i < numberOfThreads; i++)
	{
		ranges[i] = i * positionsPerThread;
	}
	vector<thread> threads;

	//Creating the Threads
	for (int i = 0; i < numberOfThreads - 1; i++)
	{
		threads.emplace_back(&Analyser::bijectionTestFromTo, this, ranges[i], ranges[i + 1] - 1);
	}
	threads.emplace_back(&Analyser::bijectionTestFromTo, this, ranges[numberOfThreads - 1], numberOfPositions - 1);

	for (auto& t : threads)
	{
		t.join();
	}

	//if no return false up to this point, then bijective
	stringstream ss;
	ss.str(string());
	ss << "Bijection Test completed. If no error then mapping is bijective." << endl;
	logCout(ss);
}

void Analyser::bijectionTestFromTo(uint64_t from, uint64_t to)
{
	for (uint64_t posID = from; posID <= to; posID++)
	{
		Position p = Position::decodePosition(posID);
		uint64_t id = p.encodePosition();

		if (id != posID) 
		{
			stringstream ss;
			ss.str(string());
			ss << "Error on posID: " << posID << "\n";
			logCout(ss);
			break;
		}
	}
}
