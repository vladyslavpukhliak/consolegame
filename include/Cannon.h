#pragma once
#include <string>

class Cannon
{
public:
	Cannon(const std::string& tile);
	void SetPosition(int x, int y);
	void SetDirection(int x, int y);
	void GetPosition(int& x, int& y) const;
	void GetDirection(int& x, int& y) const;
	std::string GetTile() const { return _tile; };


private:
	std::string _tile;

	int _x;
	int _y;
	int _directionX;
	int _directionY;

};

