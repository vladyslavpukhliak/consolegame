#pragma once
#include <vector>
#include <string>
#include "Player.h"
#include "Enemy.h"

class Level
{
public:
	int buttonPlate = 0;
	Level();

	void load(std::string file, Player &player);
	static bool isBusy();
	void Draw();
	void setPlayerName(std::string nickname);

	void Move(char input, Player &player);
	void UpdateEnemies(Player &player);

	char GetTile(int x, int y);
	void SetTile(int x, int y, char tile);

private:
	void TryGo(Player& player, int targetX, int targetY);
	void TryEnemyGo(Player& player, int index, int targetX, int targetY);
	void BattleEnemy(Player& player, int targetX, int targetY);

private:
	std::string playerName;
	std::vector <std::string> _levelData;
	std::vector <Enemy> _enemies;
};

