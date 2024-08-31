#include "GamePlayer.h"
#pragma once
class Game
{
private:
	GamePlayer* player0;
	GamePlayer* player1;
	Position currentPosition;

public:
	/*
	Constructor taking two Gameplayers.
	First GamePlayer parameter (player0) will be player to move in the starting position.
	*/
	Game(GamePlayer* player0, GamePlayer* player1);

	Position getCurrentPosition();

	~Game();

	/*
	Simulates a game between the two assigned GamePlayers. Return value is the ID of winning player.
	0 = false; 1 = true
	*/
	bool playGame();
};

