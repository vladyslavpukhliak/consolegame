#pragma once
#include <string>

class Enemy
{
public:
	Enemy(std::string name, char tile, int level, int attack, int health, int experience);

	void SetPosition(int x, int y);
	void GetPosition(int& x, int& y);
	std::string GetName() { return _name; };
	char GetTile() { return _tile; };

	int attack();
	int TakeDamage(int attack);
	
	// AI move command
	char GetMove(int playerX, int playerY);

private:
	std::string _name;
	char _tile;

	int _level;
	int _attack;
	// int _attackRange;
	int _health;
	int _experience;

	int _x;
	int _y;
};

