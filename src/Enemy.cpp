#include "Enemy.h"
#include "Graphics.h"
#include "Services.h"

#include <string>
#include <random>
#include <ctime>


Enemy::Enemy(std::string name, std::string line, std::string art, std::string conversation,
	std::vector <std::string> deathLines,
	std::string tile, int color, int level, int attack, int health, int experience, int visibleRange,
	bool isMovable, bool isFriendly, bool fear, bool isUnbeatable) {
	_name = name;
	_art = art;
	_conversation = conversation;
	_deathLines = deathLines;
	_tile = tile;
	_color = color;
	_line = line;
	_level = level;
	_attack = attack;
	_health = health;
	_experience = experience;
	_visibleRange = visibleRange;
	_isMovable = isMovable;
	_isFriendly = isFriendly;
	_fear = fear;
	_isUnbeatable = isUnbeatable;
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
	static std::default_random_engine randomEngine(time(NULL));
	std::uniform_int_distribution<int> attackRoll(0, _attack);

	return attackRoll(randomEngine);
}
int Enemy::TakeDamage(int damage) {
	_health -= damage;
	if (_health <= 0) {
		return _experience;
	}
	return 0;
}

char Enemy::GetMove(int playerX, int playerY) {
	if (!_isMovable) return ' ';
	static std::default_random_engine randomEngine(time(NULL));
	std::uniform_int_distribution<int> moveRoll(0, 6);

	int dx = _x - playerX;
	int dy = _y - playerY;
	int adx = abs(dx);
	int ady = abs(dy);
	int distance = adx + ady;

	if (distance <= _visibleRange && !_isFriendly) {
		if (!_fear) {
			if (!isSpotted && _line != "") Services::graphics().addMessage(Services::graphics().Utf8ToAnsi(_line));
			isSpotted = true;
			// ќриг?нальна лог?ка Ч до гравц¤
			// наближенн¤ гравц¤ до enemy (така лог?ка залишаЇтьс¤ незм?нною, можна зм?нити лише w,a,s,d)
			if (adx > ady) {
				return (dx > 0) ? 'a' : 'd';
			}
			else {
				return (dy > 0) ? 'w' : 's';
			}
		}
		else {
			// Ќовий блок Ч в?д гравц¤ (протилежний рух)
			if (adx > ady) {
				return (dx > 0) ? 'd' : 'a';  // навпаки
			}
			else {
				return (dy > 0) ? 's' : 'w';  // навпаки
			}
		}
	}
	else isSpotted = false;

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
		return ' ';
		break;
	}
}
