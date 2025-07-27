#pragma comment(lib, "winmm.lib")
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
// Standard library headers
#include <conio.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime> 
#include <fstream>
#include <sstream>

// custom headers
#include "GameSystem.h"
#include "Graphics.h"
#include "Message.h"

// RapidJSON
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"
#include "include/rapidjson/stringbuffer.h"
#include "include/rapidjson/prettywriter.h"

// Windows header
#include <windows.h>

#pragma region JSON Read/Write

using namespace rapidjson;

// Будь ласка винесіть це в окремий клас. ЯКОГО БІСА ВОНО ТУТ РОБЕ
// Function to write player info into a JSON file
void GameSystem::writePlayerData(const std::string& filename,
	const std::string& nickname, int exp, int level, int money, int deaths) {

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember("nickname", Value(nickname.c_str(), allocator), allocator);
	doc.AddMember("experience", exp, allocator);
	doc.AddMember("level", level, allocator);
	doc.AddMember("money", money, allocator);
	doc.AddMember("deaths", deaths, allocator);

	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);  // PrettyWriter makes it more readable
	doc.Accept(writer);

	std::ofstream ofs(filename);
	if (ofs.is_open()) {
		ofs << buffer.GetString();
		ofs.close();
		//std::cout << "Data written to " << filename << std::endl;
	}
	else {
		std::cerr << "\nFailed to open file for writing." << std::endl;
	}
}

// Function to read the player name from a JSON file
std::string readPlayerName(const std::string& filename) {
	std::ifstream ifs(filename);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open file for reading." << std::endl;
		return "";
	}

	std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	Document doc;
	doc.Parse(json.c_str());

	if (doc.HasMember("nickname") && doc["nickname"].IsString()) {
		return doc["nickname"].GetString();
	}

	return "";
}

// ТИМЧАСОВІ РІШЕННЯ
int readPlayerLevel(const std::string& filename) {
	std::ifstream ifs(filename);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open file for reading." << std::endl;
		return 1;
	}

	std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	Document doc;
	doc.Parse(json.c_str());

	if (doc.HasMember("level") && doc["level"].IsInt()) {
		return doc["level"].GetInt();
	}

	return 1;
}
int readPlayerDeaths(const std::string& filename) {
	std::ifstream ifs(filename);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open file for reading." << std::endl;
		return 0;
	}

	std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	Document doc;
	doc.Parse(json.c_str());

	if (doc.HasMember("deaths") && doc["deaths"].IsInt()) {
		return doc["deaths"].GetInt();
	}

	return 0;
}
int readPlayerMoney(const std::string& filename) {
	std::ifstream ifs(filename);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open file for reading." << std::endl;
		return 1;
	}

	std::string json((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	Document doc;
	doc.Parse(json.c_str());

	if (doc.HasMember("money") && doc["money"].IsInt()) {
		return doc["money"].GetInt();
	}

	return 1;
}

#pragma endregion

// TODO: запис імені з json

// TODO: рандомне прізвисько надодачу до імені, наприклад Вася Непийпиво YES
// TODO: відокремити логіку читання файла в окремий клас шоб не сорити

Level _level;
Message messageList;
Player _player;
bool isDone = false;
bool isBadEnd = false;
bool isPaused = false;

bool GameSystem::isGameOver() { return isBadEnd; }
void GameSystem::BadEnding() { isBadEnd = true;  }
void GameSystem::PauseTheGame() { isPaused = true; };
void GameSystem::UnPauseTheGame() { isPaused = false; };
// Constructor sets up the game
//GameSystem::GameSystem(std::string levelFile) {
//
//	_player.init(1, 10, 100, 10);
//
//	_level.load(levelFile, _player);
//}

void enemy_thread_func()
{
	while (!isDone && !isBadEnd)
	{
		if (isPaused) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue; 
		}

		//if(!Message::isBusy) 
		// Оновлення позицій ворогів
		//if (Level::isBusy) {
		_level.UpdateEnemies(_player);
		//}

		// Зупинка потоку на 500 мілісекунд
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

// шо це за діч? давайте зробимо information і відмальовку рівня в одному кадрі. (TODO)
void draw_thread() {
	while (!isDone && !isBadEnd)
	{
		if (isPaused) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		// add check if enemies ever was initialy
		if ((_level.buttonPlatesWereHere && _level.buttonPlate == 0) || (_level.enemiesWereHere && _level.getEnemiesCount() == 0)) isDone = true;
		messageList.checkExpiredmessageList();
		_level.Draw();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

std::string getFileName(const fs::directory_entry& file) { return file.path().stem().string(); }

#pragma region Вибір прізвиська
void GameSystem::displayPrompt() {
	std::cout << "\x1B[2J\x1B[H";
	printf("Настисніть ESC, щоб повернутися до головного меню.\n");
	printf("Настискаючи Лівий або Правий Alt на клавіатурі, оберіть прізвисько своєму герою.\n");
	printf("Ви можете обрати тільки прізвисько натиснувши Enter, або ще дописати повне ім'я.\n");
	printf("Запишіть своє ім'я (залишається %d з 28 символів): ", 28 - nameLength);
	std::cout << (randomNickname) << " " << (name);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// встановлюємо межі для Index для масиву
void GameSystem::updateIndex(int& currentIndex, const unsigned int lastIndex) {
	if (currentIndex < 0) currentIndex = lastIndex;
	else if (currentIndex > lastIndex) currentIndex = 0;
}

void GameSystem::updateNickname() {
	updateIndex(nicknameIndex, nicknames.size() - 1);
	randomNickname = nicknames[nicknameIndex];
	nameLength = name.length() + randomNickname.length() + 1;
	displayPrompt();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// вибір нікнейму клавішами Alt поки не натиснуто ентер
bool GameSystem::nicknamePicked() {
	while (true) {
		// Left Alt
		if (GetAsyncKeyState(VK_LMENU) & 0x8000) {
			nicknameIndex--;
			updateNickname();
		}
		// Right Alt
		else if (GetAsyncKeyState(VK_RMENU) & 0x8000) {
			nicknameIndex++;
			updateNickname();
		}

		if (_kbhit()) {
			int key = _getch();
			if (key == '\r') {  // Enter
				return true;
				break;
			}
			else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
				return false;
				break;
			}
			else if (GetAsyncKeyState(VK_BACK) & 0x8000) { // Backspace
				if (!name.empty()) {
					name.pop_back();
					std::cout << "\b \b";
					nameLength = name.length() + randomNickname.length();
				}
			}
			else {  // Друковані символи
				if (nameLength < 28) {
					name += (char)key;
					nameLength = name.length() + randomNickname.length() + 1;
					displayPrompt();
				}
			}
		}
	}
}
#pragma endregion


#pragma region SAVINGS Operations

// Будь ласка винесіть це в окремий клас. ЯКОГО БІСА ВОНО ТУТ РОБЕ

// Вивід збережень та часу змін з масиву
void GameSystem::printRangeOfSavings(const std::vector<SaveFile>& savingFiles) {
	unsigned int i = 0, j = 0, k = 0;
	std::string savingName;
	size_t lastSaving;
	std::vector<std::vector<Saving>> tenSavingsArray;

	// Розділення на групи по 10 (ініціалізація масиву)
	size_t groupCount = (savingFiles.size() + 9) / 10;
	tenSavingsArray.resize(groupCount);

	// Забороняємо вихід індексу за межі усіх сторінок
	updateIndex(savingsIndex, tenSavingsArray.size() - 1);


	// запис до масиву елементів та розділення їх в групи по 10 
	for (const auto& saveFile : savingFiles) {

		savingName = getFileName(saveFile.entry);

		auto rawLastModifiedTime = saveFile.time;
		auto cftime =
			std::chrono::time_point_cast<std::chrono::system_clock::duration>(
				rawLastModifiedTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

		auto time_t = std::chrono::system_clock::to_time_t(cftime);

		tenSavingsArray[i].push_back({ ++k, savingName, time_t });
		j++;
		if (j == 10) {
			i++;
			j = 0;
		}
	}

	// Шапка
	std::cout << "\x1B[2J\x1B[H";
	printf("Настисніть ESC, щоб повернутися до головного меню.\n");
	printf("Настискаючи Лівий або Правий Alt на клавіатурі, гортайте сторінку зі збереженнями.\n");

	lastSaving = tenSavingsArray[savingsIndex].size()-1;

	printf("Відображення збережень %d - %d з %d:\n\n", tenSavingsArray[savingsIndex][0].count,
		tenSavingsArray[savingsIndex][lastSaving].count, savingFiles.size());

	printf("\t%-35s%s\n\n", "Збереження гравця", "Дата збереження");


	// Вивід збережень поточної сторінки
	for (const auto& saving : tenSavingsArray[savingsIndex]) {
		std::ostringstream oss;
		oss << '\t' << saving.count << ". " << std::left << std::setw(32) << saving.savingName
			<< std::put_time(std::localtime(&saving.time_t), "%Y-%m-%d %H:%M") << std::endl;

		// не парні рядки позначаємо іншим кольором для легкого сприйняття великого потоку інформації
		std::cout << ((saving.count % 2) ? graphicsManager.colorize(oss.str(), 30, 47) : oss.str());
		//std::cout << oss.str();
	}
	printf("\n\nОберіть збереження: ");
}


bool GameSystem::savingHasBeenSelected(const std::string& folderPath, const std::string& extension) {
	unsigned int i = 0;
	std::vector<SaveFile> savingFiles;

	// Запис збережень у масив
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file() && entry.path().extension() == extension
			&& getFileName(entry).size() <= 28) {
			auto lastModifiedTime = fs::last_write_time(entry);
			savingFiles.push_back({ entry, lastModifiedTime });
		}
	}

	// Сортування масиву за датою BUBBLE SORT
	for (int i = 0; i < savingFiles.size() - 1; i++) {
		for (int j = 0; j < savingFiles.size() - i - 1; j++) {
			if (savingFiles[j].time < savingFiles[j + 1].time) {
				std::swap(savingFiles[j], savingFiles[j + 1]);
			}
		}
	}

	// Безпосередньо вивід збережень
	printRangeOfSavings(savingFiles);


	int choice;
	std::string input;
	while (true) {
		choice = 0;
		input = "";		

		// Читаємо введення посимвольно
		while (true) {
			// Left Alt
			if (GetAsyncKeyState(VK_LMENU) & 0x8000) {
				savingsIndex--;
				printRangeOfSavings(savingFiles);
				std::cout << input;
			}
			// Right Alt
			else if (GetAsyncKeyState(VK_RMENU) & 0x8000) {
				savingsIndex++;
				printRangeOfSavings(savingFiles);
				std::cout << input;
			}
			if (_kbhit()) {
				int key = _getch();

				
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { // ESC 
					return false; // повернутися на головне меню.
				}
				else if (key == '\r') { // Enter
					if (!input.empty()) {
						try {
							choice = std::stoi(input);
							break; // Вихід з циклу введення
						}
						catch (const std::exception&) {
							std::cout << "\nНеправильний формат числа. Спробуйте знову: ";
							input = "";
						}
					}
				}
				else if (key == '\b' && !input.empty()) { // Backspace
					input.pop_back();
					std::cout << "\b \b"; // Стираємо символ на екрані
				}
				else if (key >= '0' && key <= '9') { // Цифри
					input += key;
					std::cout << (char)key; // Показуємо введену цифру
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		choice--;
		if (choice < 0 || choice > savingFiles.size()-1)
		{
			std::cout << "\x1B[2J\x1B[H";
			printf("Ви ввели неправильний номер збереження. Щоб спробувати знову натисніть Enter...\n");
			_getch();
			printRangeOfSavings(savingFiles);
		}
		else {
			SaveFile our_player = savingFiles[choice];
			name = our_player.entry.path().stem().string();
			return true;
		}
	}
}

bool hasFilesWithExtension(const std::string& folderPath, const std::string& extension, const std::string& filename = "") {
	// Перевірка на існування папки
	fs::path dirPath(folderPath);
	if (!fs::exists(dirPath) || !fs::is_directory(dirPath)) {
		return false;
	}

	if (!filename.empty()) {
		for (const auto& entry : fs::directory_iterator(folderPath)) {
			if (entry.is_regular_file() &&
				entry.path().extension() == extension &&
				getFileName(entry) == filename) {
				return true;
			}
		}
		return false;
	}

	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file() && entry.path().extension() == extension) {
			return true;
		}
	}

	return false;
}

// Read all player nicknames
void GameSystem::getPlayerNicknames() {
	std::ifstream namesFile("assets/settings/names.json");
	if (!namesFile) {
		std::cerr << "Не вдалося відкрити файл names.json" << std::endl;
	}

	std::stringstream buffer;
	buffer << namesFile.rdbuf();
	std::string jsonContent = buffer.str();

	// Парсимо JSON
	rapidjson::Document document;
	if (document.Parse(jsonContent.c_str()).HasParseError()) {
		std::cerr << "Помилка парсингу JSON" << std::endl;
	}

	// Перевіряємо, що це масив
	if (!document.IsArray()) {
		std::cerr << "JSON не є масивом" << std::endl;
	}

	// Виводимо всі елементи масиву

	for (const auto& item : document.GetArray()) {
		if (item.IsString() && item.GetStringLength() <= 28) {
			nicknames.push_back(item.GetString());
		}
	}

	// Read txt file 
	/*std::string nickname;
	while (!namesFile.eof())
	{
		getline(namesFile, nickname);
		if(nickname.length() <=28)
			nicknames.push_back(nickname);
	}
	namesFile.close();*/
}

#pragma endregion


bool GameSystem::newGame() {
	// Боже, що це за страховисько
	getPlayerNicknames();

	while (true) {
		// restart randomizer
		srand(static_cast<unsigned>(time(nullptr)));

		nicknameIndex = rand() % nicknames.size();
		randomNickname = nicknames[nicknameIndex]; // гравцю пропонується рандомне прізвисько на початку гри
		nameLength = name.length() + randomNickname.length() + 1;

		displayPrompt();
		if (!nicknamePicked()) return false;

		std::cout << "\n";
		graphicsManager.print("Ваше ім'я: ", 500);

		// якщо ім'я не було введено - використати лиш прізвисько.
		name += (!name.empty() ? " " + randomNickname : randomNickname);

		graphicsManager.print(name, 500);

		std::cout << "\n\n";
		graphicsManager.print("Підтвердити вибір? ", 100);
		std::cout << "(Enter/ESC)";

		// Очищаємо буфер
		while (_kbhit()) {
			_getch();
		}
		// Очікуємо натискання клавіші
		int key = _getch();

		if (key == '\r') { // Enter
			if (!hasFilesWithExtension("./assets/savings/", ".json", name)) {
				std::string filename = "./assets/savings/" + name + ".json";

				// Запис даних у файл!!!!!
				writePlayerData(filename, randomNickname, 0, 1, 0, 0);
				return true;
			}
			else {
				std::cout << "\x1B[2J\x1B[H";
				printf("Таке збереження вже існує. Щоб спробувати знову натисніть Enter...\n");
				_getch();
				// Перезаписати? подумати чи треба реалізувати
				name.clear();
			}
		}
		else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { // ESC
			name.clear();
			// продовжуємо цикл
		}
	}
}

std::string GameSystem::initMainArt(const std::string& path) {
	std::ifstream artFile;

	artFile.open(path);
	if (artFile.fail()) {
		std::cout << "\x1B[2J\x1B[H";
		std::cerr<< "No such file: \""+ path +"\"\n";
		Sleep(600);
		exit(1);
	}
	
	std::string lines, line;
	while (getline(artFile, line))
	{
		lines += line + '\n';
	}
	artFile.close();
	return lines;
}

std::vector<std::string> splitByNewline(const std::string& input) {
	std::vector<std::string> lines;
	std::istringstream stream(input);
	std::string line;
	while (std::getline(stream, line)) {
		lines.push_back(line);
	}
	return lines;
}

// Вивід головного меню
void GameSystem::printMainMenu() {
	// Показуємо меню
	std::cout << "\x1B[2J\x1B[H";
	printf("1. Продовжити бавитись\n");
	printf("2. Нова гра\n");
	printf("3. Покинути бавку\n");

	std::cout << artToPrint;
	std::vector<std::string> lines = splitByNewline(qrToPrint);
	for (size_t i = 0; i < lines.size(); i++)
	{
		graphicsManager.setCursorPos(75, i);
		printf(lines[i].c_str());
	}
}

// Логіка головного меню
void GameSystem::mainMenuLogic() {
	graphicsManager.SetWindowSize(145, 40);
	printMainMenu();

	while (true) {
		if (_kbhit()) {
			int key = _getch();

			switch (key) {

			case '\r':
			case '1':   // клавіша «1»
				if (!hasFilesWithExtension("./assets/savings", ".json"))
				{
					std::cout << "\x1B[2J\x1B[H";
					std::cout << "Збережень не знайдено. Щоб повернутися до головного меню натисніть будь-яку клавішу...\n";
					_getch();
					printMainMenu();
				}

				else if (savingHasBeenSelected("./assets/savings", ".json"))
					return; // Вихід після вибору збереження
				else printMainMenu();

				break;

			case '2':   // клавіша «2»
				// Вихід з функції, якщо newGame() повернула true
				if (newGame())
					return;
				else printMainMenu();

				break;

			case '3':   // клавіша «3»
				std::exit(0);
				return;
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

std::string getMusicFiles(const std::string& folderPath, const std::string& extension) {
	std::vector<std::string> arrayOfFiles;

	// Запис знайдених файлів у масив
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file() && entry.path().extension() == extension) {
			arrayOfFiles.push_back(entry.path().string());
		}
	}

	srand(static_cast<unsigned>(time(nullptr)));
	unsigned int randomIndex = rand() % arrayOfFiles.size();

	return arrayOfFiles[randomIndex];
}

void GameSystem::saveAfterDeath(std::string& savingName) {
	std::string filename = "./assets/savings/" + savingName + ".json";
	writePlayerData(filename, readPlayerName(filename), 0, readPlayerLevel(filename),
		_player.GetAvailableMoney(), readPlayerDeaths(filename) + 1);
}

void GameSystem::RunGame() {
	while (true) {

		isDone = false;
		isBadEnd = false;
		name = "";
		_level.clear();
		graphicsManager.SetWindowTitle("ConsoleGame		forged by Vladyslav Pukhliak");

		// Інтерактив з гравцем. Інтерфейс головного меню. Frontend
		mainMenuLogic();


		// можна додати різних фраз в файл і ними оперувати на рандом
		std::cout << "\x1B[2J\x1B[H";

		// і це Боже, що це за страховисько!
		graphicsManager.print("Побачимо що ти собою представляєш, ", 0);
		graphicsManager.print(name, 1000);
		graphicsManager.print("...", 1500, 3000);

		graphicsManager.unprint("Побачимо що ти собою представляєш, " + name + "...\b", 100);
		std::string filename = "./assets/savings/" + name + ".json";
		printf("%d зафіксованих смертей на це ім'я.", readPlayerDeaths(filename));
		Sleep(3000);

		// Завершальна логіка, починаємо вводити ігрові дані безпосередньо у гру. Backend

		// Відкриття вже існуючого файла (тут суцільна каша)
		_level.setPlayerName(name);
		_player.init(1, 10, 100, 10, readPlayerMoney(filename)); // задання початкових параметрів гравцеві!


		system("cls");
		std::string levelName = "level" + std::to_string(readPlayerLevel(filename));

		if (hasFilesWithExtension("./assets/Levels/", ".txt", levelName))
		{
			_level.load("./assets/Levels/" + levelName + ".txt", _player);
		}
		else {
			printf("Такого збереження не існує або ж ви пройшли усі рівні (Enter щоб продовжити)...");
			_getch();
			continue;
		}



		Graphics graphics;
		graphics.init();

		/*std::cout << "\n\n" << getMusicFiles("assets/Music/", ".mp3");
		Sleep(10000);*/

		std::string playThis = "open \"" + getMusicFiles("assets/Music/", ".mp3") + "\" type mpegvideo alias leMusic";
		mciSendStringA(playThis.c_str(), NULL, 0, NULL);
		mciSendStringA("play leMusic", NULL, 0, NULL);

		std::thread enemy_thread(enemy_thread_func);
		std::thread draw(draw_thread);


		// Цей цикл продовжує працювати після смерті ГГ!
		while (!isDone) // isBadEnd?
		{
			//if (!Level::isBusy()) {
			_level.Move(_getch(), _player);
			//_level.Draw();
		//}
		// Затримка головного потоку на 16 мілісекунд (приблизно 60 кадрів в секунду)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		system("cls");
		printf("You win!!! Перехід до головного меню (4s)...");

		// Запис даних у файл!!!!!
		int playerLevel = readPlayerLevel(filename);
		writePlayerData(filename, readPlayerName(filename), 0, ++playerLevel, _player.GetAvailableMoney(), readPlayerDeaths(filename));
		// 0 в кінці що означає "смертей" замінити на справжні смерті.

		mciSendStringA("close leMusic", NULL, 0, NULL);
		// 
		// Очікування завершення потоку з рухом ворогів
		enemy_thread.join();
		draw.join();

		// КРИТИЧНО: Встановлюємо прапорці для завершення потоків
		isDone = true;
		isBadEnd = true; // Щоб точно завершилися



		Sleep(4000);
		system("cls");
	}
}
