#include "Enemy.h"
#include <string>
#include <random>
#include <ctime>

Enemy::Enemy(string name, char tile, int level, int attack, int health, int expirience) {
	_name = name;
	_tile = tile;
	_level = level;
	_attack = attack;
	_health = health;
	_expirience = expirience;
}

void Enemy::SetPosition(int x, int y) {
	_x = x;
	_y = y;
}

void Enemy::GetPosition(int& x, int& y) {
	x = _x;
	y = _y;
}

int Enemy::attack() {
	static default_random_engine randomEngine(time(NULL));
	uniform_int_distribution<int> attackRoll(0, _attack);

	return attackRoll(randomEngine);
}
int Enemy::TakeDamage(int damage) {
	_health -= damage;
	if (_health <= 0) {
		return _expirience;
	}
	return 0;
}

char Enemy::GetMove(int playerX, int playerY) {
	static default_random_engine randomEngine(time(NULL));
	uniform_int_distribution<int> moveRoll(0, 6);

	int dx = _x - playerX;
	int dy = _y - playerY;
	int adx = abs(dx);
	int ady = abs(dy);
	int distance = adx + ady;

	if (distance <= 5) {
		// Moving along X axis
		if (adx > ady) {
			if (dx > 0) return 'a';
			else return 'd';
		}
		// Moving along Y axis
		else {
			if (dy > 0) return 'w';
			else return 's';
		}
	}
	
	int randMove = moveRoll(randomEngine);
	switch (randMove)
	{
	case 0:
		return 'a';
		break;
	case 1:
		return 'w';
		break;
	case 2:
		return 's';
		break;
	case 3:
		return 'd';
		break;
	default:
		return '.';
		break;
	}
}
