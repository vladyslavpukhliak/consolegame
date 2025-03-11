#pragma once
#include "Level.h"
#include "Player.h"
#include <string>

class GameSystem
{
public:
	GameSystem(std::string levelFile);

	void RunGame();
	static void BadEnding();
	static bool isGameOver();
};
