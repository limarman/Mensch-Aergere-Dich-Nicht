#include "Game.h"
#include <random>



Game::Game(GamePlayer* player0, GamePlayer* player1)
{
	this->player0 = player0;
	this->player1 = player1;
}

bool Game::playGame() 
{
	random_device rd;
	mt19937 mt(rd()); //random seeding
	uniform_int_distribution<int> dist(1, 6);

	while (true) {
		int dieroll = dist(mt); //roll the die
		if (currentPosition.getTurn()) //if player1's turn
		{
			currentPosition = player1->chooseSuccessor(currentPosition, dieroll);
			if (currentPosition.getPlayer(1)->hasFinished()) 
			{
				return true;
			}
		}
		else 
		{
			currentPosition = player0->chooseSuccessor(currentPosition, dieroll);
			if (currentPosition.getPlayer(0)->hasFinished())
			{
				return false;
			}
		}
	}

}

Position Game::getCurrentPosition() 
{
	return currentPosition;
}


Game::~Game()
{
}
