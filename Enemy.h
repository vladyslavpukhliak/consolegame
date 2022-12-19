#pragma once
#include <string>
using namespace std;

class Enemy
{
public:
	Enemy(string name, char tile, int level, int attack, int health, int expirience);

	void SetPosition(int x, int y);
	void GetPosition(int& x, int& y);
	string GetName() { return _name; };
	char GetTile() { return _tile; };

	int attack();
	int TakeDamage(int attack);
	
	// AI move command
	char GetMove(int playerX, int playerY);

private:
	string _name;
	char _tile;

	int _level;
	int _attack;
	// int _attackRange;
	int _health;
	int _expirience;

	int _x;
	int _y;
};

