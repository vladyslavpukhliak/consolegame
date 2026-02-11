#include "Cannon.h"

Cannon::Cannon(const std::string& tile) : _tile(tile), _x(0), _y(0), _directionX(0), _directionY(0) {
}

void Cannon::SetPosition(int x, int y) {
	_x = x;
	_y = y;
}

void Cannon::GetPosition(int& x, int& y) const {
	x = _x;
	y = _y;
}
void Cannon::SetDirection(int x, int y) {
	_directionX = x;
	_directionY = y;
}

void Cannon::GetDirection(int& x, int& y) const {
	x = _directionX;
	y = _directionY;
}


