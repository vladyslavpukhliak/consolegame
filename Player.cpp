#include <random>
#include <ctime>
#include <thread>
#include <chrono>
#include "Player.h"
#include "Graphics.h"
#include <windows.h>

Graphics graphicManager;

Player::Player() {
	_x = 0;
	_y = 0;
}

void Player::init(int level, int attack, int health, int experience) {
	_level = level;
	_attack = attack;
	_health = health;
	_experience = experience;
}

int Player::attack() {
	static std::default_random_engine randomEngine(time(NULL));
	std::uniform_int_distribution<int> attackRoll(0, _attack);

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

void Player::AddExperience(int xp) {
	_experience += xp;

	// Level up !
	while (_experience > 50) {
		_level++;
		_attack += 10;
		_health += 1;
		_experience -= 50;

		graphicManager.addMessage("Leveled up! Your Level: " + std::to_string(_level));
	}
}

int Player::TakeDamage(int damage) {
	_health -= damage;
	if (_health <= 0) {
		return 1;
	}
	return 0;
}
