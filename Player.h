#pragma once
class Player
{
public:
	Player();
	void init(int level, int _attack, int health, int expirience);

	int attack();
	int TakeDamage(int damage);

	void SetPosition(int x, int y);
	void GetPosition(int &x, int &y);

	void AddExpirience(int xp);

private:
	int _level;
	int _attack;
	int _health;
	int _expirience;

	int _x;
	int _y;
};

