#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <conio.h>
#include "Level.h"
#include "Enemy.h"
#include "Message.h"
#include "Graphics.h"
#include "FileSystem.h"
#include "GameSystem.h"
#include "entities.h"
#include <windows.h>

//Message messageList;
Graphics graphicsManager;
Message message;
bool busy = false;

Level::Level() {

}

// Loads the level
void Level::load(std::string fileName, Player& player) {
	std::ifstream file;

	// Перевірка файлу
	file.open(fileName);
	if (file.fail()) {
		perror(fileName.c_str());
		Sleep(600);
		exit(1);
	}

	// Завантажити данні з файлу в масив
	std::string line;
	while (getline(file, line)) {
		_levelData.push_back(line);
	}
	file.close();


	// Я СЮДА ВСТАВИВ І ТРЕБА ЗНАЙТИ КРАЩЕ МІСЦЕ
	/*system("cls");
	Cannon cannon = loadCannonParams("assets/settings/props.json");
	printf("Cannon name: %s\n", cannon.tile[0]);
	
	Sleep(6000);*/

	// Ініціалізація рівня
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
			case 'X': // Moveable box
				buttonPlate++;
				break;
			}
		}
	}
}

bool Level::isBusy() { return busy; };

// Відобразити кадр
void Level::Draw() {
	if (busy) return;
	busy = true;

	//for (int i = 0; i < _levelData.size(); i++) {
	//	//if (message.isBusy()) return;
	//	graphicsManager.setCursorPos(0, i);
	//	std::cout << _levelData[i];
	//}
			std::string line;
	char tile;
	graphicsManager.setCursorPos(0, 0);
	for (int i = 0; i < _levelData.size(); i++) {
			//if (message.isBusy()) return;
			//graphicsManager.setCursorPos(0, i);

			//line.reserve(_levelData[i].size() * 12); // Резервуємо з запасом на ANSI коди ?

			for (int j = 0; j < _levelData[i].size(); j++) {
				tile = _levelData[i][j];

				switch (tile) {
				case 'B': // Box
					line += graphicsManager.colorizeTile(tile, 33);
					break;
				case '$': // Money
					line += graphicsManager.colorizeTile(tile, 32);
					break;
				case 'S': // Snake
					line += graphicsManager.colorizeTile(tile, 32);
					break;
				case 'r': // Rozbiynyk
					line += graphicsManager.colorizeTile(tile, 33);
					break;
				case 'O': // Ork
					line += graphicsManager.colorizeTile(tile, 31);
					break;
				case 'X': // Moveable box
					line += graphicsManager.colorizeTile(tile, 35);
					break;
				case '@': // Player
					line += graphicsManager.colorizeTile(tile, 33);
					break;
				default:
					line += graphicsManager.colorizeTile(tile, 37);
					break;
				}
			}

			line += "\n";
	}
	std::cout<<line;
	busy = false;
	if (!GameSystem::isGameOver()) {
		message.checkExpiredmessageList();
		message.printmessageList();
	}
	//printf("\n");
}

void Level::setPlayerName(std::string nickname) {
	playerName = nickname;
}

void Level::Move(char input, Player& player) {

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
		graphicsManager.addMessage("Invalid input!");
		break;
	}
}

char Level::GetTile(int x, int y) { return _levelData[y][x]; }
void Level::SetTile(int x, int y, char tile) { _levelData[y][x] = tile; }

void Level::TryGo(Player& player, int targetX, int targetY) {

	int playerX;
	int playerY;
	player.GetPosition(playerX, playerY);

	int horizontal = targetX - playerX;
	int vertical = targetY - playerY;


	// Map boundaries
	if (targetX <= 0 || targetY <= 0 ||
		targetX >= _levelData[targetY].size() - 1 ||
		targetY >= _levelData.size() - 1) {

		graphicsManager.addMessage("You ran into the wall!");
		return;
	}

	// Символ попереду гравця
	char tileAhead = GetTile(targetX + horizontal, targetY + vertical);

	char nextTile = GetTile(targetX, targetY);

	switch (nextTile) {
	case '#': // Internal walls
		graphicsManager.addMessage("You ran into the wall!");
		break;

	case '.':
		player.SetPosition(targetX, targetY);
		SetTile(playerX, playerY, '.');
		SetTile(targetX, targetY, '@');
		break;

	case 'B': // Move Box logic

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

		graphicsManager.addMessage("+600 UAH");
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
	std::string enemyName;
	std::string attackInfo;
	int attackRoll;
	int attackResult;

	player.GetPosition(playerX, playerY);

	for (int i = 0; i < _enemies.size(); i++) {
		_enemies[i].GetPosition(enemyX, enemyY);
		enemyName = _enemies[i].GetName();
		if (targetX == enemyX && targetY == enemyY) {
			// Battle !
			attackRoll = player.attack();
			attackInfo = "Player attacked " + enemyName + " with a roll of: " + std::to_string(attackRoll);
			graphicsManager.addMessage(attackInfo);

			attackResult = _enemies[i].TakeDamage(attackRoll);
			if (attackResult != 0) {
				SetTile(targetX, targetY, '.');
				Draw();

				graphicsManager.addMessage("Enemy died!\n");

				// Removing the enemy
				_enemies[i] = _enemies.back();
				_enemies.pop_back();
				i--;
				// Add enemy's death sound
				Sleep(600);
				player.AddExperience(attackResult);

				return;
			}
			// Enemy's turn !
			attackRoll = _enemies[i].attack();

			attackInfo = enemyName + " attacked You with a roll of: " + std::to_string(attackRoll);
			graphicsManager.addMessage(attackInfo);
			attackResult = player.TakeDamage(attackRoll);

			if (attackResult != 0) {
				SetTile(playerX, playerY, '~');
				// Sound of Player death
				graphicsManager.addMessage("You died!");
				Draw();
				GameSystem::BadEnding();
				Sleep(600);
				system("CLS");

				// Loads the art
				std::ifstream artFile;

				artFile.open("assets/Art/Death.txt");
				if (artFile.fail()) {
					perror("No such file: \"Art/Death.txt\"");
					Sleep(600);
					exit(1);
				}

				std::string line;
				// Метод swap() для повного звільнення пам'яті
				std::vector<std::string>().swap(_levelData); //_levelData.clear();


				std::string wordToReplace = "nickname", tempWord;
				size_t lineY = 0, replaceX = 0, replaceY = 0,
					wordSize = wordToReplace.length(), nameLength = playerName.length();

				while (getline(artFile, line)) {
					lineY++;

					if (line.find(wordToReplace) != std::string::npos) {
						// Координати для курсора
						replaceX = line.find(wordToReplace);
						replaceY = lineY - 1;

						// Заміна nickname на ім'я гравця
						uint8_t charsToAdd;
						if (nameLength != wordSize) {
							if (nameLength % 2)
								playerName += ' ';

							nameLength = playerName.length(); // оновлюємо дані

							if (nameLength < wordSize) {
								charsToAdd = wordSize - nameLength;
								charsToAdd /= 2;
								for (size_t i = 0; i < charsToAdd; i++)
									playerName = ' ' + playerName;
								for (size_t i = 0; i < charsToAdd; i++)
									playerName = playerName + ' ';
							}
							else if (nameLength > wordSize) {
								charsToAdd = nameLength - wordSize;
								charsToAdd /= 2;
								replaceX -= charsToAdd;
							}
						}
						
						//line.replace(replaceX, wordSize - 1, playerName);
					}
					_levelData.push_back(line);
				}
				artFile.close();
				Draw();
				//printf("test: %d, %d, %s.", replaceX, replaceY, playerName.c_str());
				graphicsManager.setCursorPos(replaceX, replaceY);
				printf(playerName.c_str());
				graphicsManager.setCursorPos(0, lineY);
				Sleep(600);
				_getch();

				exit(0);
			}

			return;
		}
	}
}

void Level::UpdateEnemies(Player& player) {
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
