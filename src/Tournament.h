#include "GamePlayer.h"
#include <atomic>
#pragma once
class Tournament
{

private:
	static const int numberOfThreads = 2;
	int numberOfGames;
	GamePlayer* player0;
	GamePlayer* player1;

	/*
	Adds to the reference the number of wins encountered
	*/
	void playGames(int numberOfGames, atomic<int>& numberOfWins);


public:

	Tournament(GamePlayer* player0, GamePlayer* player1, int numberOfGames);

	void startTournament();

	~Tournament();
};

