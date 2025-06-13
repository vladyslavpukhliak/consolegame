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
#include <filesystem>

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

namespace fs = std::filesystem;
using namespace rapidjson;

// Function to write player info into a JSON file
void GameSystem::writePlayerData(const std::string& filename,
	const std::string& nickname, int exp, int level, double money) {

	Document doc;
	doc.SetObject();
	Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember("nickname", Value(nickname.c_str(), allocator), allocator);
	doc.AddMember("experience", exp, allocator);
	doc.AddMember("level", level, allocator);
	doc.AddMember("money", money, allocator);

	StringBuffer buffer;
	PrettyWriter<StringBuffer> writer(buffer);  // PrettyWriter makes it more readable
	doc.Accept(writer);

	std::ofstream ofs(filename);
	if (ofs.is_open()) {
		ofs << buffer.GetString();
		ofs.close();
		std::cout << "Data written to " << filename << std::endl;
	}
	else {
		std::cerr << "Failed to open file for writing." << std::endl;
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

// Function to read the player level from a JSON file
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

// TODO: запис імені з json

// TODO: рандомне прізвисько надодачу до імені, наприклад Вася Непийпиво YES
// TODO: відокремити логіку читання файла в окремий клас шоб не сорити
// TODO: в оформлені цієї курсової вкажи якусь інфу з Чистого Коду і додай у список літератури.

Level _level;
Message messageList;
Player _player;
bool isDone = false;
bool isBadEnd = false;

bool GameSystem::isGameOver() { return isBadEnd; }
void GameSystem::BadEnding() { isBadEnd = true; }
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
		//if(!Message::isBusy) 
		// Оновлення позицій ворогів
		//if (Level::isBusy) {
		_level.UpdateEnemies(_player);
		//}

		// Зупинка потоку на 500 мілісекунд
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
void draw_thread() {
	while (!isDone && !isBadEnd)
	{
		if (_level.buttonPlate == 0) isDone = true;
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
	printf("Запишіть своє ім'я (залишається %d з 28 символів): ", 28 - nameLength);
	std::cout << (randomNickname) << " " << (name);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
// встановлюємо межі для nicknameIndex для масиву
void GameSystem::updateNicknameIndex() {
	const unsigned int lastNickname = nicknames.size() - 1;
	if (nicknameIndex < 0) nicknameIndex = lastNickname;
	else if (nicknameIndex > lastNickname) nicknameIndex = 0;
}
// вибір нікнейму клавішами Alt поки не натиснуто ентер
bool GameSystem::nicknamePicked() {
	while (true) {
		// Left Alt
		if (GetAsyncKeyState(VK_LMENU) & 0x8000) {
			nicknameIndex += 1;
			updateNicknameIndex();
			randomNickname = nicknames[nicknameIndex];
			nameLength = name.length() + randomNickname.length() + 1;
			displayPrompt();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		// Right Alt
		else if (GetAsyncKeyState(VK_RMENU) & 0x8000) {
			nicknameIndex -= 1;
			updateNicknameIndex();
			randomNickname = nicknames[nicknameIndex];
			nameLength = name.length() + randomNickname.length() + 1;
			displayPrompt();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
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



struct SaveFile {
	fs::directory_entry entry;
	fs::file_time_type time;
};




// Вивід збережень та часу змін з масиву
void printRangeOfSavings(const std::vector<SaveFile>& savingFiles) {
	unsigned int i = 0;
	std::string savingName;

	// Шапка
	std::cout << "\x1B[2J\x1B[H";
	printf("Настисніть ESC, щоб повернутися до головного меню.\n");
	printf("Відображення перших 10 збережень:\n\n");
	printf("\t%-35s%s\n\n", "Збереження гравця", "Дата збереження");

	for (const auto& saveFile : savingFiles) {

		savingName = getFileName(saveFile.entry);

		auto rawLastModifiedTime = saveFile.time;
		auto cftime =
			std::chrono::time_point_cast<std::chrono::system_clock::duration>(
				rawLastModifiedTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());

		auto time_t = std::chrono::system_clock::to_time_t(cftime);

		if (i < 10) { // це жах. змінити на range >= a && < b
			printf("\t%d. %-32s", ++i, savingName.c_str()); // це жах. змінити %-цифриs на щось краще!!!!!!
			std::cout << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M") << std::endl;
		}
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
		if (choice < 0 || choice > savingFiles.size())
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
		std::cerr << "Не вдалося відкрити файл test.json" << std::endl;
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
				writePlayerData(filename, randomNickname, 0, 1, 0);
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

// Вивід головного меню
void printMainMenu() {
	// Показуємо меню
	std::cout << "\x1B[2J\x1B[H";
	if (hasFilesWithExtension("./assets/savings", ".json")) {
		printf("1. Продовжити\n");
	}
	else {
		std::cout << "No savings found in the folder.\n";
	}
	printf("2. Нова гра\n");
	printf("3. Вихід\n");
}

// Логіка головного меню
void GameSystem::mainMenuLogic() {
	printMainMenu();

	while (true) {
		if (_kbhit()) {
			int key = _getch();

			switch (key) {

			case '\r':
			case '1':   // клавіша «1»
				if (savingHasBeenSelected("./assets/savings", ".json"))
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

void GameSystem::RunGame() {

	// Інтерактив з гравцем. Інтерфейс головного меню. Frontend
	mainMenuLogic();


	// можна додати різних фраз в файл і ними оперувати на рандом
	std::cout << "\x1B[2J\x1B[H";

	// і це Боже, що це за страховисько! виправити так щоб там де крапки їм окрема була пауза в методі.
	graphicsManager.print("Побачимо що ти собою представляєш, ", 0);
	graphicsManager.print(name, 1000);
	graphicsManager.print("...", 2000, 3000);

	graphicsManager.unprint("Побачимо на що ти здатен, " + name + "...", 100);



	// Завершальна логіка, починаємо вводити ігрові дані безпосередньо у гру. Backend

	_level.setPlayerName(name);
	_player.init(1, 10, 100, 10); // задання початкових параметрів гравцеві!

	// Відкриття вже існуючого файла (тут суцільна каша. зверху реєстрація її треба зробити вибірковою)
	std::string filename = "./assets/savings/" + name + ".json";
	_level.load("./assets/Levels/level" + std::to_string(readPlayerLevel(filename)) + ".txt", _player);
	system("cls");

	Graphics graphics;
	graphics.init();


	//mciSendStringA("open \"Music/moon_crystals.mp3\" type mpegvideo alias level1.mp3", NULL, 0, NULL);
	//mciSendStringA("play level1.mp3", NULL, 0, NULL);

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
	printf("You win!!!");
	//mciSendStringA("close level1.mp3", NULL, 0, NULL);

	// Очікування завершення потоку з рухом ворогів
	enemy_thread.join();
	draw.join();
}
