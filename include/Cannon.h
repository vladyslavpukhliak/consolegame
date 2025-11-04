#pragma once
#include <string>

class Cannon
{
public:
	Cannon(std::string tile);
	void SetPosition(int x, int y);
	void SetDirection(int x, int y);
	void GetPosition(int& x, int& y);
	void GetDirection(int& x, int& y);


private:
	std::string _tile;
	// int _attackRange;

	int _x;
	int _y;
	int directionX;
	int directionY;

};

