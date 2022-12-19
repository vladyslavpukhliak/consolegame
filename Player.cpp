#include <Windows.h>

#include "Player.h"
#include <random>
#include <ctime>

using namespace std;

Player::Player() {
	_x = 0;
	_y = 0;
}

void Player::init(int level, int attack, int health, int expirience) {
	_level = level;
	_attack = attack;
	_health = health;
	_expirience = expirience;
}

int Player::attack() {
	static default_random_engine randomEngine(time(NULL));
	uniform_int_distribution<int> attackRoll(0, _attack);

	return attackRoll(randomEngine);
}

void Player::SetPosition(int x, int y) {
	_x = x;
	_y = y;
}

void Player::GetPosition(int &x, int &y) {
	x = _x;
	y = _y;
}

void Player::AddExpirience(int xp) {
	_expirience += xp;

	// Level up !
	while (_expirience > 50) {
		_level++;
		_attack += 10;
		_health += 1;
		_expirience -= 50;

		printf("Leveled up! Your Level: %d\n", _level);
		Sleep(600);
	}
}

int Player::TakeDamage(int damage) {
	_health -= damage;
	if (_health <= 0) {
		return 1;
	}
	return 0;
}
