#pragma once
#include "Level.h"
#include "Player.h"
#include <string>

class GameSystem
{
public:
	GameSystem(string levelFile);

	void RunGame();
	void enemy_thread_func();
private:
	Level _level;
	Player _player;
};

