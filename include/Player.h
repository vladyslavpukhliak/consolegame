#pragma once
class Player
{
public:
	Player();
	void init(int level, int _attack, int health, int experience, int money);

	int attack();
	int TakeDamage(int damage);

	void SetPosition(int x, int y);
	void GetPosition(int &x, int &y) const;

	void AddExperience(int xp);

	bool TryCharge(int priceToCharge);
	void TopUp(int amount) { _money += abs(amount);	}
	size_t GetAvailableMoney() const { return _money; }

private:
	int _level;
	int _attack;
	int _health;
	int _experience;
	int _money;

	int _x;
	int _y;
};

