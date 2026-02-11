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
#include "constants.h"
#include "Services.h"


// RapidJSON
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

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
Player _player;
bool isDone = false;
bool isBadEnd = false;
bool isPaused = false;
bool DrawThreadIsPaused = false;

bool GameSystem::isGameOver() { return isBadEnd; }
void GameSystem::BadEnding() { isBadEnd = true;  }
void GameSystem::PauseTheGame() { isPaused = true; };
void GameSystem::UnPauseTheGame() { isPaused = false; };
void GameSystem::PauseDrawThread() { DrawThreadIsPaused = true; };
void GameSystem::UnPauseDrawThread() { DrawThreadIsPaused = false; };


void cannon_thread_func()
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
		_level.UpdateCannon(_player);
		//}

		// Зупинка потоку на 2 секунди
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	}
}

void missile_thread_func()
{
	while (!isDone && !isBadEnd)
	{
		if (isPaused) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		// Ці закоментовані рядки - це спроба уникнути накладання потоків один на одного,
		// тоді я не розумів чому виникають конфлікти, тепер розумію що це через те що в кожному класі
		// я створюю окрему копію об'єкта Message і GraphicsManager
		// В принципі це не страшно якщо використовувати якісь окремі їхні функції
		// Але загалом такий підхід відстійний і треба робити через посилання або вказівники
		// 
		//if(!Message::isBusy) 
		// Оновлення позицій ворогів
		//if (Level::isBusy) {
		_level.UpdateMissiles(_player);
		//}

		// Пауза потоку
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

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
		if (DrawThreadIsPaused) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		// add check if enemies ever was initialy
		if ((_level.buttonPlatesWereHere && _level.buttonPlate == 0) ||
			(_level.enemiesWereHere && _level.getEnemiesCount() == 0)) isDone = true;
		_level.Draw(_player);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

std::string getFileName(const fs::directory_entry& file) { return file.path().stem().string(); }

#pragma region ChoosingNickname

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
		std::cout << ((saving.count % 2) ? Services::graphics().colorize(oss.str(), 30, 47) : oss.str());
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
		Services::graphics().print("Ваше ім'я: ", 500);

		// якщо ім'я не було введено - використати лиш прізвисько.
		name += (!name.empty() ? " " + randomNickname : randomNickname);

		Services::graphics().print(name, 500);

		std::cout << "\n\n";
		Services::graphics().print("Підтвердити вибір? ", 100);
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
//std::string GameSystem::initMainArt(const std::filesystem::path& path) {
//}

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
	printf("3. Як бавитись?\n");
	printf("4. Більше контенту\n");
	printf("0. Покинути бавку\n");

	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
	std::cout << artToPrint;
	std::vector<std::string> lines = splitByNewline(qrToPrint);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Services::graphics().setCursorPos(75, i);
		printf(lines[i].c_str());
	}
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

// Логіка головного меню
void GameSystem::mainMenuLogic() {
	Services::graphics().SetWindowSize(145, 40);
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
				std::cout << "\x1B[2J\x1B[H";
				std::cout << "Розділ знаходиться в розробці...\n";
				std::cout << "Підтримай розробника кавою =)\n\n";
				std::cout << "Щоб продовжити натисніть Enter...\n";
				_getch();
				printMainMenu();
				break;

			case '4':{   // клавіша «4»
				std::string cmd = "start \"\" \"assets/settings/See more content webpage/more.html\"";
				int ret = std::system(cmd.c_str());

				if (ret != 0) {
					std::cerr << "Не вдалося відкрити документ, код: " << ret << "\n";
					std::exit(1);
				}
			}
				break;

			case '0':   // клавіша «0»
				std::string playThis = "open \"assets/Music/Menu/MyFirstLetterToYou.mp3\" type mpegvideo alias leMusic";
				mciSendStringA(playThis.c_str(), NULL, 0, NULL);
				mciSendStringA("play leMusic", NULL, 0, NULL);
				system("cls");
				std::string kiss = R"(
                    .:+!++:::.  .:u+::.
                 !!!X:!X<!!!<!#%?!!~XX!!!!:
             :<!!X!!!!:!!>?~!~:<!~!!!?!!!X!!!:
           <!!!%!!!!!~!!!!!!<!!!!!!!!!!!!!!!X!!:
         <!!!!!!!!!!:<<<~~~!~~~~!~~~~!~!<!:!!!!!!:
       :!!!!!!!!!~~~~~~~~~~~~~~~~~~~~~~~~~~<!~!!!!!.
     .!\!!!!!~~~~~~~~~.io@$$$$N$$$bou. `~~~~~~~~:!!!:
    <~~~~~~~~~~~~   ~*?******"#R$$$$$R"$Mx ~~~~~~~~!~~
   '`~~~~~~          M#?#"?#**!@**?""??*MX     `~~~~~~~
    `~~:::::::::~:~~:<:H!!:~<:~:<~::>:<:!::::::::::<<~~
       ~!!!!!?!!MMXHM@$5@HM%8kdNh!HNRZ7$@MR$N!!?!<"~~
         ~!!!!!!!!!?!!R?MMM!#$T*M!RMSMXM7!!!!!!!\~~
           ~~~!!!!!~!!!!!!!!!<!!!!:!!!!!!!!<!~!~~
              ~~~~!!!!~!~!~~~!>!:~!!!!!!!!!~~~
                 `~~~~~~!<!~~!<!!~~!::~~~~~
                         ~~~~~~~~~~~~

				)";
				
				std::cout << kiss;
				std::cout << "Au revoir, архітекторе світів!\n";

				Sleep(12000);
				std::exit(0);
				return;
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


// TODO: Перевірка чи існує шлях чи є в ньому файли
std::string GameSystem::getMusicFiles(const std::string& folderPath, const std::string& extension) {
	std::vector<std::string> arrayOfFiles;

	// Запис знайдених файлів у масив
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file() && entry.path().extension() == extension) {
			arrayOfFiles.push_back(entry.path().stem().string());
		}
	}

	// If no files found, return empty string (caller should handle this)
	if (arrayOfFiles.empty()) {
		return std::string();
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

void loadLevel(const std::string& filename, std::string musicName, Player& _player)
{
	std::string levelName = "level" + std::to_string(readPlayerLevel(filename));

	if (hasFilesWithExtension("./assets/Levels/", ".txt", levelName))
	{
		_level.load("./assets/Levels/" + levelName + ".txt", musicName, _player);
	}
	else {
		printf("Такого збереження не існує або ж ви пройшли усі рівні (Enter щоб продовжити)...\n");
		_getch();
		return;
	}
}

void GameSystem::RunGame() {
	while (true) {

		isDone = false;
		isBadEnd = false;
		name = "";
		_level.clear();
		Services::graphics().SetWindowTitle(TITLE + " | Main menu");

		// Інтерактив з гравцем. Інтерфейс головного меню. Frontend
		mainMenuLogic();


		// можна додати різних фраз в файл і ними оперувати на рандом
		std::cout << "\x1B[2J\x1B[H";

		// і це Боже, що це за страховисько!
		Services::graphics().print("Побачимо що ти собою представляєш, ", 0);
		Services::graphics().print(name, 1000);
		//Services::graphics().print("...", 1500, 3000);

		Services::graphics().unprint("Побачимо що ти собою представляєш, " + name + "...\b", 100);
		std::string filename = "./assets/savings/" + name + ".json";
		printf("%d зафіксованих смертей на це ім'я.", readPlayerDeaths(filename));
		//Sleep(3000);

		// Завершальна логіка, починаємо вводити ігрові дані безпосередньо у гру. Backend

		// Відкриття вже існуючого файла (тут суцільна каша)

		// Відкриття Settings json, увімкнення музики
		std::ifstream ifs("assets/settings/settings.json");
		if (!ifs.is_open()) {
			std::cerr << "Не вдалося відкрити settings.json\n";
		}
		rapidjson::IStreamWrapper isw(ifs);
		rapidjson::Document doc;
		doc.ParseStream(isw);
		if (doc.HasParseError() || !doc.IsObject()) {
			std::cerr << "Невірний формат settings.json\n";
		}
		bool canPlayMusic = false;
		const char* playMusicKey = "playMusic";

		if (doc.HasMember(playMusicKey) && doc[playMusicKey].IsInt()) {
			canPlayMusic = (doc[playMusicKey].GetInt() != 0);
		}
		ifs.close();

		std::string musicPath = "assets/Music/";
		std::string musicName = getMusicFiles(musicPath, ".mp3");
		if (canPlayMusic) {
			std::string playThis = "open \""+ musicPath + musicName + ".mp3\" type mpegvideo alias leMusic";
			mciSendStringA(playThis.c_str(), NULL, 0, NULL);
			mciSendStringA("play leMusic", NULL, 0, NULL);
		}

		_level.setPlayerName(name);
		_player.init(1, 10, 100, 10, readPlayerMoney(filename)); // задання початкових параметрів гравцеві!


		std::cout << "\x1B[2J\x1B[H";
		loadLevel(filename, musicName, _player);


		Graphics graphics;
		graphics.init();

		//Sleep(10000);

		std::thread missile_thread(missile_thread_func);
		std::thread cannon_thread(cannon_thread_func);
		std::thread enemy_thread(enemy_thread_func);
		std::thread draw(draw_thread);


		// Цей цикл продовжує працювати після смерті ГГ!
		while (!isDone) // isBadEnd?
		{
			char key = _level.Move(_getch(), _player);

			if (key == 'r' || key == 'R') {
				// --- Рестарт рівня ---
				PauseTheGame();
				PauseDrawThread();
				Services::message().clearMessageList();
				std::cout << "\x1B[2J\x1B[H";
				//printf("Перезапуск рівня...\n");
				_level.clear();
				graphics.setCursorPos(0, 0);
				graphics.init();

				_level.setPlayerName(name);
				_player.init(1, 10, 100, 10, readPlayerMoney(filename)); // задання початкових параметрів гравцеві!
				loadLevel(filename, musicName, _player);
				UnPauseDrawThread();
				UnPauseTheGame();
				continue;
			}

			// Затримка для зменшення навантаження на CPU
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
		missile_thread.join();
		cannon_thread.join();
		enemy_thread.join();
		draw.join();

		// КРИТИЧНО: Встановлюємо прапорці для завершення потоків
		isDone = true;
		isBadEnd = true; // Щоб точно завершилися



		Sleep(4000);
		system("cls");
	}
}
