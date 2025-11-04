#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>


#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "Player.h"
#include "Cannon.h"
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

	char Move(char input, Player &player);
	void UpdateEnemies(Player &player);
	void UpdateCannon(Player &player);
	void UpdateMissiles(Player &player);

	std::string GetTile(int x, int y);
	void SetTile(int x, int y, std::string tile);
	void clear();
	unsigned int getEnemiesCount();
	bool buttonPlatesWereHere = false, enemiesWereHere = false;

private:
	void TryGo(Player& player, int targetX, int targetY);
	void TryEnemyGo(Player& player, int index, int targetX, int targetY);
	void BattleEnemy(Player& player, int targetX, int targetY);
	void TryCannonShoot(Player& player, int index, int targetX, int targetY);
	void TryMissileGo(Player& player, int index, int targetX, int targetY);
	void CheckPlayerDeath(int attackResult, int playerX, int playerY);

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

	struct CannonDirection {
		int dx;
		int dy;
	};

	struct CannonTemplate {
		std::vector<std::string> tile;
		std::vector<CannonDirection> directions;
		std::string name;
		int color;
		std::string description;/*
		std::string playerDeathLine;*/
	};

private:
	std::string playerName;
	std::vector <std::string> _initialMaze;
	std::vector<std::vector<std::string>> _levelData;
	std::vector <Enemy> _enemies;
	std::vector <Cannon> _cannon;
	std::vector <Cannon> _missiles;
	std::unordered_map<std::string, CannonTemplate> cannonTemplates;
	std::unordered_map<std::string, EnemyTemplate> enemyTemplates;
};

