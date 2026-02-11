#include "Enemy.h"
#include "Graphics.h"
#include "Services.h"
#include "constants.h"

#include <string>
#include <random>
#include <chrono>


Enemy::Enemy(const std::string& name, const std::string& line, const std::string& art, 
	const std::string& conversation, const std::vector<std::string>& deathLines,
	const std::string& tile, int color, int level, int attack, int health, 
	int experience, int visibleRange, bool isMovable, bool isFriendly, 
	bool fear, bool isUnbeatable)
	: _name(name), _line(line), _art(art), _conversation(conversation),
	  _deathLines(deathLines), _tile(tile), _color(color), _level(level),
	  _attack(attack), _health(health), _experience(experience),
	  _visibleRange(visibleRange), _isMovable(isMovable), _isFriendly(isFriendly),
	  _fear(fear), _isUnbeatable(isUnbeatable), _x(0), _y(0), isSpotted(false)
{
}

void Enemy::SetPosition(int x, int y) {
	_x = x;
	_y = y;
}

void Enemy::GetPosition(int& x, int& y) const {
	x = _x;
	y = _y;
}

int Enemy::attack() {
	static std::default_random_engine randomEngine(
		static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())
	);
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
	static std::default_random_engine randomEngine(
		static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())
	);
	std::uniform_int_distribution<int> moveRoll(RANDOM_MOVE_MIN, RANDOM_MOVE_MAX);

	int dx = _x - playerX;
	int dy = _y - playerY;
	int adx = abs(dx);
	int ady = abs(dy);
	int distance = adx + ady;

	if (distance <= _visibleRange && !_isFriendly) {
		if (!_fear) {
			if (!isSpotted && _line != "") Services::graphics().addMessage(Services::graphics().Utf8ToAnsi(_line));
			isSpotted = true;
			// ����?������ ���?�� � �� ������
			// ���������� ������ �� enemy (���� ���?�� ��������� ����?����, ����� ��?���� ���� w,a,s,d)
			if (adx > ady) {
				return (dx > 0) ? 'a' : 'd';
			}
			else {
				return (dy > 0) ? 'w' : 's';
			}
		}
		else {
			// ����� ���� � �?� ������ (����������� ���)
			if (adx > ady) {
				return (dx > 0) ? 'd' : 'a';  // �������
			}
			else {
				return (dy > 0) ? 's' : 'w';  // �������
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
