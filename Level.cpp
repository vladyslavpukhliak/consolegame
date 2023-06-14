#include <Windows.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include "Level.h"
#include "Enemy.h"
using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD cursor_pos;

Level::Level() {

}

void Level::load(string fileName, Player &player) {

	// Loads the level
	ifstream file;

	file.open(fileName);
	if (file.fail()) {
		perror(fileName.c_str());
		Sleep(600);
		exit(1);
	}

	string line;

	while (getline(file, line)) {
		_levelData.push_back(line);
	}

	file.close();


	// Process the level
	char tile;
	for (int i = 0; i < _levelData.size(); i++) {
		for (int j = 0; j < _levelData[i].size(); j++) {
			tile = _levelData[i][j];

			switch (tile) {
			case '@': // Player
				player.SetPosition(j, i);
				break;
			case 'S': // Snake
				_enemies.push_back(Enemy("Snake", tile, 1, 5, 10, 150));
				_enemies.back().SetPosition(j, i);
				break;
			case 'r': // Rozbiynyk
				_enemies.push_back(Enemy("Rozbiynyk", tile, 2, 10, 40, 200));
				_enemies.back().SetPosition(j, i);
				break;
			case 'O': // Ork
				_enemies.push_back(Enemy("Ork", tile, 3, 45, 300, 750));
				_enemies.back().SetPosition(j, i);
				break;
			//case '$': // Million Dollar
			//	enemies.push_back(Enemy("Snake", tile, 1, 5, 10));
			//	break;
			case 'X': // Moveable box
				buttonPlate++;
				break;
			}
		}
	}
}

void Level::print() {
	cursor_pos = { 0, 0 };
	SetConsoleCursorPosition(console, cursor_pos);

	for (int i = 0; i < _levelData.size(); i++) {
		printf("%s\n", _levelData[i].c_str());
	}
	printf("\n");
}

void Level::Move(char input, Player &player) {

	int playerX;
	int playerY;
	player.GetPosition(playerX, playerY);


	switch (input)
	{
	case 'w': case 'W':
		TryGo(player, playerX, playerY - 1);
		break;

	case 's': case 'S':
		TryGo(player, playerX, playerY + 1);
		break;

	case 'a': case 'A':
		TryGo(player, playerX - 1, playerY);
		break;

	case 'd': case 'D':
		TryGo(player, playerX + 1, playerY);
		break;

	default:
		printf("Invalid input!");
		Sleep(600);
		break;
	}
}

char Level::GetTile(int x, int y) { return _levelData[y][x]; }
void Level::SetTile(int x, int y, char tile) { _levelData[y][x] = tile;}

void Level::TryGo(Player& player, int targetX, int targetY) {

	int playerX;
	int playerY;
	player.GetPosition(playerX, playerY);

	int horizontal = targetX - playerX;
	int vertical = targetY - playerY;
	char tileAhead = GetTile(targetX + horizontal, targetY + vertical);

	char nextTile = GetTile(targetX, targetY);

	switch (nextTile) {
	case '#':
		cursor_pos = { 40, 1 };
		SetConsoleCursorPosition(console, cursor_pos);
		printf("You ran into the wall!");
		Sleep(600);
		break;
	case '.':
		player.SetPosition(targetX, targetY);
		SetTile(playerX, playerY, '.');
		SetTile(targetX, targetY, '@');
		break;
	case 'B':
		// Move Box logic
		
		if (tileAhead == '.' || tileAhead == 'X') {
			player.SetPosition(targetX, targetY);
			SetTile(playerX, playerY, '.');
			SetTile(targetX, targetY, '@');
			SetTile(targetX + horizontal, targetY + vertical, 'B');
		}
		if (tileAhead == 'X') buttonPlate--;

		break;
	case '$':
		player.SetPosition(targetX, targetY);
		SetTile(playerX, playerY, '.');
		SetTile(targetX, targetY, '@');

		cursor_pos = { 40, 1 };
		SetConsoleCursorPosition(console, cursor_pos);
		printf("+600 UAH");
		Sleep(600);
		break;

	default:
		BattleEnemy(player, targetX, targetY);
		break;
	}
}
void Level::TryEnemyGo(Player& player, int index, int targetX, int targetY) {

	int playerX;
	int playerY;
	int enemyX;
	int enemyY;
	_enemies[index].GetPosition(enemyX, enemyY);
	player.GetPosition(playerX, playerY);

	char nextTile = GetTile(targetX, targetY);

	switch (nextTile) {
	case '@':
		BattleEnemy(player, enemyX, enemyY);
		break;
	case '.':
		_enemies[index].SetPosition(targetX, targetY);
		SetTile(enemyX, enemyY, '.');
		SetTile(targetX, targetY, _enemies[index].GetTile());
		break;

	default:

		break;
	}
}

void Level::BattleEnemy(Player& player, int targetX, int targetY) {

	int enemyX;
	int enemyY;
	int playerX;
	int playerY;
	string enemyName;
	int attackRoll;
	int attackResult;

	player.GetPosition(playerX, playerY);

	for (int i = 0; i < _enemies.size(); i++) {
		_enemies[i].GetPosition(enemyX, enemyY);
		enemyName = _enemies[i].GetName();
		if (targetX == enemyX && targetY == enemyY) {
			// Battle !
			attackRoll = player.attack();
			printf("Player attacked %s with a roll of: %d\n", enemyName.c_str(), attackRoll);
			attackResult = _enemies[i].TakeDamage(attackRoll);
			if (attackResult != 0) {
				SetTile(targetX, targetY, '.');
				print();
				cursor_pos = { 40, 1 };
				SetConsoleCursorPosition(console, cursor_pos);
				printf("Enemy died!\n");

				// Removing the enemy
				_enemies[i] = _enemies.back();
				_enemies.pop_back();
				i--;

				Sleep(600);
				player.AddExpirience(attackResult);
				
				return;
			}
			// Enemy's turn !
			attackRoll = _enemies[i].attack();
			cursor_pos = { 40, 1 };
			SetConsoleCursorPosition(console, cursor_pos);
			printf("%s attacked You with a roll of: %d\n", enemyName.c_str(), attackRoll);
			attackResult = player.TakeDamage(attackRoll);

			if (attackResult != 0) {
				SetTile(playerX, playerY, '~');
				print();
				printf("You died!\n");
				Sleep(600);
				system("CLS");

				// Loads the art
				ifstream artFile;

				artFile.open("Art/Death.txt");
				if (artFile.fail()) {
					perror("No such file: \"Art/Death.txt\"");
					Sleep(600);
					exit(1);
				}

				string line;
				// Метод swap() для повного звільнення пам'яті
				vector<string>().swap(_levelData); //_levelData.clear();

				while (getline(artFile, line)) {
					_levelData.push_back(line);
				}
				print();
				Sleep(600);
				artFile.close();

				exit(0);
			}

			return;
		}
	}
}

void Level::UpdateEnemies(Player & player) {
	char aiMove;
	int playerX;
	int playerY;
	int enemyX;
	int enemyY;

	player.GetPosition(playerX, playerY);
	for (int i = 0; i < _enemies.size(); i++) {
		aiMove = _enemies[i].GetMove(playerX, playerY);
		_enemies[i].GetPosition(enemyX, enemyY);
		switch (aiMove)
		{
		case 'w':
			TryEnemyGo(player, i, enemyX, enemyY - 1);
			break;

		case 's':
			TryEnemyGo(player, i, enemyX, enemyY + 1);
			break;

		case 'a':
			TryEnemyGo(player, i, enemyX - 1, enemyY);
			break;

		case 'd':
			TryEnemyGo(player, i, enemyX + 1, enemyY);
			break;
		}
	}
}
