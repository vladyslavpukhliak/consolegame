#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>


#include "include/rapidjson/document.h"
#include "include/rapidjson/istreamwrapper.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"

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
	void clear();
	unsigned int getEnemiesCount();
	bool buttonPlatesWereHere = false, enemiesWereHere = false;

private:
	void TryGo(Player& player, int targetX, int targetY);
	void TryEnemyGo(Player& player, int index, int targetX, int targetY);
	void BattleEnemy(Player& player, int targetX, int targetY);

	struct EnemyTemplate {
		std::string name, line, art, conversation;
		std::vector <std::string> deathLines;
		int color;
		int level;
		int attack;
		int health;
		int experience;
		int visibleRange;
		bool movable;
		bool friendly;
		bool fear;
		bool unbeatable;
	};

private:
	std::string playerName;
	std::vector <std::string> _levelData;
	std::vector <Enemy> _enemies;
	std::unordered_map<char, EnemyTemplate> enemyTemplates;
};

