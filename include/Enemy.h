#pragma once
#include <vector>
#include <string>

class Enemy
{
public:
	Enemy(const std::string& name, const std::string& line, const std::string& art, 
		const std::string& conversation, const std::vector<std::string>& deathLines, 
		const std::string& tile, int color, int level, int attack, int health, 
		int experience, int visibleRange, bool isMovable, bool isFriendly, 
		bool fear, bool isUnbeatable);

	void SetPosition(int x, int y);
	void GetPosition(int& x, int& y) const;
	std::string GetName() const { return _name; };
	std::string GetTile() const { return _tile; };

	int attack();
	int TakeDamage(int attack);
	
	// AI move command
	char GetMove(int playerX, int playerY);

	std::string _name, _line, _art, _conversation;
	std::vector <std::string> _deathLines;
	bool _isFriendly, _fear, _isUnbeatable, _isMovable;
	int _visibleRange, _color;

private:
	std::string _tile;

	int _level;
	int _attack;
	int _health;
	int _experience;


	int _x;
	int _y;

	bool isSpotted = false;
};

