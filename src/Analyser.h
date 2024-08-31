#pragma once
#include "MadN.h"
#include <string>
#include "GamePlayer.h"

/*
struct for representing the output of the similarity analysis.
holding:
number of non-finished situations (where the game is still not decided, that is where no player has reached a winning configuration)
number of non-trivial situations (situations where there is more than possible move)
number of situations where two GamePlayers are choosing the same move
*/
struct similarityStats {
	uint64_t non_finished_situations;
	uint64_t non_trivial_situations;
	uint64_t same_output_situations;
};

class Analyser 
{

private:

	vector<float>* probabilities;
	int numberOfThreads;

	void compareGamePlayersFromTo(uint64_t from, uint64_t to, GamePlayer* gp1, GamePlayer* gp2,
		similarityStats* stats);

	void bijectionTestFromTo(uint64_t from, uint64_t to);


public:

	/*
	Initializes Analyser class with "numberOfThreads" threads and the proababilities to work on.
	*/
	Analyser(int numberOfThreads, vector<float>* probabilities);

	/*
	Analyses on how many situations the two GamePlayers gp1 and gp2 are choosing the same move.
	Output of the statistics in form of the similarityStats struct (additionally counting the number of
	non-finished situations and non-trivial-situations for relativity of similarity).
	*/
	similarityStats compareGamePlayers(GamePlayer* gp1, GamePlayer* gp2);


	/*
	Analyses on how many situations the two GamePlayers gp chooses the same move as the optimal Player.
	Output of the statistics in form of the similarityStats struct (additionally counting the number of
	non-finished situations and non-trivial-situations for relativity of similarity).
	*/
	similarityStats compareToOptimal(GamePlayer* gp);


	/*
	Tests whether the function mapping ids onto positions is bijective.
	Outputs at least one ID where this is not the case.
	*/
	void idMapBijectionTest();


};