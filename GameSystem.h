#pragma once
#include "Level.h"
#include "Player.h"
#include <string>

using namespace std;

class GameSystem
{
public:
	GameSystem(string levelFile);

	void RunGame();
	void PlayerInput();
private:
	char get_char(float time_limit);
private:
	Level _level;
	Player _player;
};

