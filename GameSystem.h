#pragma once
#include "Level.h"
#include "Player.h"
#include <string>

class GameSystem
{
public:
	GameSystem(string levelFile);

	void RunGame();
	static void BadEnding();
	static bool isGameOver();
};
