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
#include "DialogueSystem.h"
#include "entities.h"
#include <windows.h>

//Message messageList;
Graphics graphicsManager;
Message message;
GameSystem gameSys;
bool busy = false;

Level::Level() {

}

unsigned int Level::getEnemiesCount() { return _enemies.size(); }

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


	// ЧИТАЄМО enemies.json
	std::ifstream ifs("assets/settings/enemies.json");
	if (!ifs.is_open()) {
		std::cerr << "Не вдалося відкрити enemies.json\n";
	}
	rapidjson::IStreamWrapper isw(ifs);
	rapidjson::Document doc;
	doc.ParseStream(isw);
	if (doc.HasParseError() || !doc.IsObject() || !doc["default"].IsObject()) {
		std::cerr << "Невірний формат enemies.json\n";
	}

	// Створимо мапу tileChar -> rapidjson::Value&
	const rapidjson::Value& def = doc["default"];

	for (auto it = def.MemberBegin(); it != def.MemberEnd(); ++it) {
		const rapidjson::Value& enemy_json = it->value;

		char tileChar = enemy_json["tile"].GetString()[0];

		EnemyTemplate tmpl;
		tmpl.name = it->name.GetString();
		tmpl.line = enemy_json.HasMember("line") && enemy_json["line"].IsString() ?
			enemy_json["line"].GetString() : "";
		
		// нововведення
		tmpl.art = enemy_json.HasMember("art") && enemy_json["art"].IsString() ?
			enemy_json["art"].GetString() : "";
		tmpl.conversation = enemy_json.HasMember("conversation") && enemy_json["conversation"].IsString() ?
			enemy_json["conversation"].GetString() : "";

		if (enemy_json.HasMember("deathLines") && enemy_json["deathLines"].IsArray()) {
			const rapidjson::Value& deathLinesArray = enemy_json["deathLines"];
			for (rapidjson::SizeType i = 0; i < deathLinesArray.Size(); ++i) {
				if (deathLinesArray[i].IsString()) {
					tmpl.deathLines.push_back(deathLinesArray[i].GetString());
				}
			}
		}
		
		if(tmpl.deathLines.empty()) tmpl.deathLines.push_back("Enemy трупік!\n");

		tmpl.color = enemy_json.HasMember("color") && enemy_json["color"].IsInt() ?
			enemy_json["color"].GetInt() : 37;
		tmpl.level = enemy_json.HasMember("level") && enemy_json["level"].IsInt() ?
			enemy_json["level"].GetInt() : 1;
		tmpl.attack = enemy_json.HasMember("attack") && enemy_json["attack"].IsInt() ?
			enemy_json["attack"].GetInt() : 0;
		tmpl.health = enemy_json.HasMember("health") && enemy_json["health"].IsInt() ?
			enemy_json["health"].GetInt() : 0;
		tmpl.experience = enemy_json.HasMember("experience") && enemy_json["experience"].IsInt() ?
			enemy_json["experience"].GetInt() : 0;
		tmpl.visibleRange = enemy_json.HasMember("visibleRange") && enemy_json["visibleRange"].IsInt() ?
			enemy_json["visibleRange"].GetInt() : 0;

		// нововведення
		tmpl.movable = enemy_json.HasMember("movable") && enemy_json["movable"].IsInt() ?
			(enemy_json["movable"].GetInt() == 1) : true;

		tmpl.friendly = enemy_json.HasMember("friendly") && enemy_json["friendly"].IsInt() ?
			(enemy_json["friendly"].GetInt() == 1) : false;
		tmpl.fear = enemy_json.HasMember("fear") && enemy_json["fear"].IsInt() ?
			(enemy_json["fear"].GetInt() == 1) : false;
		tmpl.unbeatable = enemy_json.HasMember("unbeatable") && enemy_json["unbeatable"].IsInt() ?
			(enemy_json["unbeatable"].GetInt() == 1) : false;

		enemyTemplates[tileChar] = tmpl;
	}






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

			if (tile == '@')
				player.SetPosition(j, i);

				// Check if this tile character corresponds to an enemy template
			else if (enemyTemplates.count(tile)) {
				const EnemyTemplate& tmpl = enemyTemplates.at(tile);

				// Create the enemy using template data
				_enemies.push_back(Enemy(tmpl.name, tmpl.line, tmpl.art, tmpl.conversation, tmpl.deathLines, tile,
					tmpl.color, tmpl.level, tmpl.attack,
					tmpl.health, tmpl.experience, tmpl.visibleRange,
					tmpl.movable, tmpl.friendly, tmpl.fear, tmpl.unbeatable));
				_enemies.back().SetPosition(j, i);
				enemiesWereHere = true;
			}

			else if (tile == 'X') { // Moveable box
				buttonPlate++;
				buttonPlatesWereHere = true;
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
	std::string line = "";
	char tile;
	graphicsManager.setCursorPos(0, 0);
	for (int i = 0; i < _levelData.size(); i++) {
		//if (message.isBusy()) return;
		//graphicsManager.setCursorPos(0, i);

		//line.reserve(_levelData[i].size() * 12); // Резервуємо з запасом на ANSI коди ?

		for (int j = 0; j < _levelData[i].size(); j++) {
			tile = _levelData[i][j];

			if (tile == '@')
				line += graphicsManager.colorize(tile, 33);

			else if (tile == 'B') // Box
				line += graphicsManager.colorize(tile, 33);

			else if (tile == '$') // Money
				line += graphicsManager.colorize(tile, 32);

			else if (tile == 'X') // Moveable box
				line += graphicsManager.colorize(tile, 35);

			else if (enemyTemplates.count(tile)) {
				const EnemyTemplate& tmpl = enemyTemplates.at(tile);
				line += graphicsManager.colorize(tile, tmpl.color);
			}

			else
				line += graphicsManager.colorize(tile, 37);

		}

		line += "\n";
	}
	std::cout << line;
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

	case ' ':
		player.SetPosition(targetX, targetY);
		SetTile(playerX, playerY, ' ');
		SetTile(targetX, targetY, '@');
		break;

	case 'B': // Move Box logic

		if (tileAhead == ' ' || tileAhead == 'X') {
			player.SetPosition(targetX, targetY);
			SetTile(playerX, playerY, ' ');
			SetTile(targetX, targetY, '@');
			SetTile(targetX + horizontal, targetY + vertical, 'B');
		}
		if (tileAhead == 'X') buttonPlate--;

		break;

	case '$':
		player.SetPosition(targetX, targetY);
		SetTile(playerX, playerY, ' ');
		SetTile(targetX, targetY, '@');

		graphicsManager.addMessage("+600 UAH");
		player.TopUp(600);
		break;

	default:
		BattleEnemy(player, targetX, targetY);
		break;
	}
}
void Level::TryEnemyGo(Player& player, int index, int targetX, int targetY) {
	// Ініціалізація координат
	int playerX, playerY, enemyX, enemyY;
	_enemies[index].GetPosition(enemyX, enemyY);
	player.GetPosition(playerX, playerY);

	// Перевіряємо пряму атаку / рух
	char nextTile = GetTile(targetX, targetY);
	if (nextTile == '@' && !_enemies[index]._isFriendly) {
		BattleEnemy(player, enemyX, enemyY);
		return;
	}
	if (nextTile == ' ') {
		_enemies[index].SetPosition(targetX, targetY);
		SetTile(enemyX, enemyY, ' ');
		SetTile(targetX, targetY, _enemies[index].GetTile());
		return;
	}

	// Усі 4 напрямки: вгору, вниз, вліво, вправо
	std::vector<std::pair<int, int>> dirs = {
		{0, -1},  // вгору
		{0, +1},  // вниз
		{-1, 0},  // вліво
		{+1, 0}   // вправо
	};

	// Збираємо тільки ті напрями, де НЕ стіна
	std::vector<std::pair<int, int>> valid;
	for (auto [dx, dy] : dirs) {
		char tile = GetTile(enemyX + dx, enemyY + dy);
		if (tile == ' ' ||
			(tile == '@' && !_enemies[index]._isFriendly)) {
			valid.emplace_back(dx, dy);
		}
	}

	if (valid.empty()) {
		// ніде йти — просто стоїмо
		return;
	}

	// Випадково обираємо один з допустимих ходів
	// TODO: ГЕНЕРАТОР фіговий, замінити на нормальний рандом.
	std::srand(static_cast<unsigned>(std::time(nullptr)));

	// …в середині TryEnemyGo, коли вже зібрали vector valid…
	size_t idx = std::rand() % valid.size();
	auto [dx, dy] = valid[idx];

	// Викликаємо ще раз для обраного напряму
	TryEnemyGo(player, index, enemyX + dx, enemyY + dy);
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
	DialogueSystem conversation;


	player.GetPosition(playerX, playerY);

	for (Enemy& enemyEntry : _enemies) {
		enemyEntry.GetPosition(enemyX, enemyY);

		// починають гамселити одне одного.		Тут перевірка на isFriendly, по суті для гравця.
		//if (targetX == enemyX && targetY == enemyY && !_enemies[i]._isFriendly) {
		if (targetX == enemyX && targetY == enemyY) {
			// TODO: Додати "категорію conversation" в дереві "Розбійник" і в ньому вже будуть ці штуки
			bool isInteractive = (!enemyEntry._isMovable && enemyEntry._art != "" 
				&& enemyEntry._conversation != "");

			if (isInteractive) {
				GameSystem::PauseTheGame();
				system("cls");

				std::ifstream artFile;
				std::string artToPrint = "", line = "";

				artFile.open(enemyEntry._art);
				if (artFile.fail()) {
					perror("No such file (Art of enemy)");
					Sleep(600);
					exit(1);
				}

				while (getline(artFile, line)) {
					artToPrint += line + '\n';
				}
				artFile.close();

				graphicsManager.setCursorPos(0,0);
				conversation.initDialogue(enemyEntry._conversation, artToPrint, player, enemyEntry);


				Sleep(2000);
				system("cls");
				graphicsManager.init();
				GameSystem::UnPauseTheGame();
			}


			// Оце будь ласка тільки після діалогу, а воно прям перед розпочинається і після.
			// Battle !
			enemyName = enemyEntry.GetName();
			if (!enemyEntry._isUnbeatable) {
				enemyEntry._isFriendly = false;
				attackRoll = player.attack();

				attackInfo = playerName + " атакував " + enemyName + " з ймовірністю: "
					+ std::to_string(attackRoll);

				graphicsManager.addMessage(attackInfo);

				attackResult = enemyEntry.TakeDamage(attackRoll);
				if (attackResult != 0) {
					SetTile(targetX, targetY, ' ');
					//Draw();


					// TODO: deathLines rand string

					size_t randomDeathLine = rand() % enemyEntry._deathLines.size();
					graphicsManager.addMessage(enemyEntry._deathLines[randomDeathLine]);
					//graphicsManager.addMessage("Enemy трупік!\n");

					// Removing the enemy
					enemyEntry = _enemies.back();
					_enemies.pop_back();
					//i--;
					// Add enemy's death sound
					Sleep(600); // закоментувати??
					player.AddExperience(attackResult);

					return;
				}
			}
			// Enemy's turn !
			attackRoll = enemyEntry.attack();

			attackInfo = enemyName + " атакував Тебе з ймовірністю: " + std::to_string(attackRoll);
			graphicsManager.addMessage(attackInfo);
			attackResult = player.TakeDamage(attackRoll);

			// Гравець - мрець.
			if (attackResult != 0) {
				SetTile(playerX, playerY, '~');
				// Sound of Player death
				graphicsManager.addMessage("Ви трупік!");
				//Draw();
				gameSys.saveAfterDeath(playerName);
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
				graphicsManager.setCursorPos(replaceX, replaceY);
				printf(playerName.c_str());
				graphicsManager.setCursorPos(0, lineY);
				Sleep(3000);
				_getch();
				/*std::cin.ignore();
				std::cin;*/

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

void Level::clear() {
	// Очищаємо дані рівня
	_levelData.clear();

	// Очищаємо ворогів
	_enemies.clear();

	// Скидаємо лічильник кнопок
	buttonPlate = 0;

	// Скидаємо ім'я гравця
	playerName = "";

	// Скидаємо прапорець зайнятості
	busy = false;
	enemiesWereHere = false;
	buttonPlatesWereHere = false;
}
