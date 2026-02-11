#include <random>
#include <chrono>
#include "Player.h"
#include "Graphics.h"
#include "Services.h"
#include "constants.h"

#include <windows.h>


Player::Player() {
	_x = 0;
	_y = 0;
}

void Player::init(int level, int attack, int health, int experience, int money) {
	_level = level;
	_attack = attack;
	_health = health;
	_experience = experience;
	_money = money;
}

int Player::attack() {
	static std::default_random_engine randomEngine(
		static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())
	);
	std::uniform_int_distribution<int> attackRoll(0, _attack);

	return attackRoll(randomEngine);
}

void Player::SetPosition(int x, int y) {
	_x = x;
	_y = y;
}

void Player::GetPosition(int &x, int &y) const {
	x = _x;
	y = _y;
}

void Player::AddExperience(int xp) {
	_experience += xp;

	// Level up !
	while (_experience > EXPERIENCE_PER_LEVEL) {
		_level++;
		_attack += ATTACK_PER_LEVEL;
		_health += HEALTH_PER_LEVEL;
		_experience -= EXPERIENCE_PER_LEVEL;

		Services::graphics().addMessage("Leveled up! Your Level: " + std::to_string(_level));
	}
}

int Player::TakeDamage(int damage) {
	_health -= damage;
	if (_health <= 0) {
		return 1;
	}
	return 0;
}

bool Player::TryCharge(int priceToCharge) {
	if (_money >= priceToCharge) {
		_money -= priceToCharge;
		return true;
	}
	return false;
}