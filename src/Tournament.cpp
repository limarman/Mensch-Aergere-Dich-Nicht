#include "Tournament.h"
#include "Game.h"
#include <thread>
#include <iostream>



Tournament::Tournament(GamePlayer* player0, GamePlayer* player1, int numberOfGames)
{
	this->player0 = player0;
	this->player1 = player1;
	this->numberOfGames = numberOfGames;
}

void Tournament::startTournament() 
{
	atomic<int> numberOfWins{ 0 };

	vector<thread> threads;

	//Creating the Threads
	for (int i = 0; i < numberOfThreads; i++)
	{
		threads.emplace_back(&Tournament::playGames, this, numberOfGames/numberOfThreads,
			ref(numberOfWins));
	}

	for (auto& t : threads)
	{
		t.join();
	}

	cout << "Games in total played: " <<
		(numberOfGames / numberOfThreads) * numberOfThreads << "\n";
	cout << "Games won by player 0: " << (numberOfWins) << "\n";
	cout << "Games won by player 1: " <<
		(numberOfGames / numberOfThreads) * numberOfThreads - numberOfWins << "\n";
}

void Tournament::playGames(int number, atomic<int>& wins) 
{
	int numberOfWins = 0;
	for (int i = 0; i < number; i++) 
	{
		Game game(player0, player1);
		bool player0Wins = !game.playGame();
		if (player0Wins) 
		{
			numberOfWins++;
		}
	}
	wins += numberOfWins;
}

Tournament::~Tournament()
{
}
