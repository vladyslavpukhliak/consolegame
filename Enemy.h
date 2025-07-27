#pragma once
#include <vector>
#include <string>

class Enemy
{
public:
	Enemy(std::string name, std::string line,std::string art, std::string conversation,
		std::vector<std::string> deathLines, 
		char tile, int color, int level, int attack, int health, int experience, int visibleRange,
		bool isMovable, bool isFriendly, bool fear, bool isUnbeatable);

	void SetPosition(int x, int y);
	void GetPosition(int& x, int& y);
	std::string GetName() { return _name; };
	//bool isFriendly() { return _isFriendly; };
	char GetTile() { return _tile; };

	int attack();
	int TakeDamage(int attack);
	
	// AI move command
	char GetMove(int playerX, int playerY);

	std::string _name, _line, _art, _conversation;
	std::vector <std::string> _deathLines;
	bool _isFriendly, _fear, _isUnbeatable, _isMovable;
	int _visibleRange, _color;

private:
	char _tile;

	int _level;
	int _attack;
	// int _attackRange;
	int _health;
	int _experience;


	int _x;
	int _y;

	bool isSpotted = false;
};

