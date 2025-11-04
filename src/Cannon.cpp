#include "Cannon.h"

Cannon::Cannon(std::string tile) {
	_tile = tile;
	//_color = color;
}

void Cannon::SetPosition(int x, int y) {
	_x = x;
	_y = y;
}

void Cannon::GetPosition(int& x, int& y) {
	x = _x;
	y = _y;
}
void Cannon::SetDirection(int x, int y) {
	directionX = x;
	directionY = y;
}

void Cannon::GetDirection(int& x, int& y) {
	x = directionX;
	y = directionY;
}


