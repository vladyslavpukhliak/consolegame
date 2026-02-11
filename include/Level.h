#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>
#include <filesystem> 


#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include "Player.h"
#include "Cannon.h"
#include "Arrow.h"
#include "Enemy.h"
#include "constants.h"

class Level
{
public:
	int buttonPlate = 0;
	Level();

	void load(const std::string& file, const std::string& musicName, Player &player);
	void loadBossfight(const std::filesystem::path& path, const std::string& art,
		const std::string& music, Player& player);
	static bool isBusy();
	void Draw(Player& player);
	void setPlayerName(const std::string& nickname);

	char Move(char input, Player &player);
	void UpdateEnemies(Player &player);
	void UpdateCannon(Player &player);
	void UpdateMissiles(Player &player);

	std::string GetTile(int x, int y) const;
	void SetTile(int x, int y, const std::string& tile);
	void clear();
	unsigned int getEnemiesCount() const;
	bool buttonPlatesWereHere = false, enemiesWereHere = false;

private:
	void TryGo(Player& player, int targetX, int targetY);
	void TryEnemyGo(Player& player, int index, int targetX, int targetY);
	void BattleEnemy(Player& player, int targetX, int targetY);
	void TryCannonShoot(Player& player, int index, int targetX, int targetY);
	void TryMissileGo(Player& player, int index, int targetX, int targetY);
	void CheckPlayerDeath(int attackResult, int playerX, int playerY, Player& player);
	void arrows_thread_func(int cooldown, Player& player);
	void UpdateArrows(Player& player);
	void TryArrowGo(Player& player, int index, int targetX, int targetY);

	struct EnemyTemplate {
		std::string name, line, art, conversation, description;
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

	struct coordinates {
		int dx;
		int dy;
	};

	struct CannonTemplate {
		std::vector<std::string> tile;
		std::vector<coordinates> directions;
		std::string name;
		int color, cannonCooldown, projectileCooldown;
		std::string description;/*
		std::string playerDeathLine;*/
	};

private:
	bool isLevelHuge = false;
	bool isBossfightNow = false;
	int legendLineNumber = 0, oldPlayerX, oldPlayerY;
	std::string fightArt, fightMusic, playerName, bossArt = "";
	std::vector <std::string> _initialMaze;
	std::vector<std::vector<std::string>> _levelData;
	std::vector<std::vector<std::string>> _oldLevelData;
	std::vector <Enemy> _enemies;
	std::vector<coordinates> _buttonPlates;
	std::vector <Cannon> _cannon;
	std::vector <Arrow> _arrows;
	std::vector <Cannon> _missiles;
	std::unordered_map<std::string, CannonTemplate> cannonTemplates;
	std::unordered_map<std::string, CannonTemplate> arrowsTemplates;
	std::unordered_map<std::string, EnemyTemplate> enemyTemplates;
};

