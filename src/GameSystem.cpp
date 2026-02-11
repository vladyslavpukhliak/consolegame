#pragma comment(lib, "winmm.lib")
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

// ���� ����� ������� �� � ������� ����. ����� ���� ���� ��� ����
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

// �������² в�����
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

// TODO: ����� ����� � json

// TODO: �������� ��������� �������� �� �����, ��������� ���� ��������� YES
// TODO: ���������� ����� ������� ����� � ������� ���� ��� �� ������

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
		// ��������� ������� ������
		//if (Level::isBusy) {
		_level.UpdateCannon(_player);
		//}

		// ������� ������ �� 2 �������
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

		// ֳ ������������� ����� - �� ������ �������� ���������� ������ ���� �� ������,
		// ��� � �� ������ ���� ��������� ��������, ����� ������ �� �� ����� �� �� � ������� ����
		// � ������� ������ ���� ��'���� Message � GraphicsManager
		// � ������� �� �� ������� ���� ��������������� ���� ����� ���� �������
		// ��� ������� ����� ����� �������� � ����� ������ ����� ��������� ��� ���������
		// 
		//if(!Message::isBusy) 
		// ��������� ������� ������
		//if (Level::isBusy) {
		_level.UpdateMissiles(_player);
		//}

		// ����� ������
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
		// ��������� ������� ������
		//if (Level::isBusy) {
		_level.UpdateEnemies(_player);
		//}

		// ������� ������ �� 500 ��������
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

// �� �� �� ��? ������� ������� information � ���������� ���� � ������ ����. (TODO)
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
	printf("���������� ESC, ��� ����������� �� ��������� ����.\n");
	printf("����������� ˳��� ��� ������ Alt �� ��������, ������ ��������� ����� �����.\n");
	printf("�� ������ ������ ����� ��������� ���������� Enter, ��� �� �������� ����� ��'�.\n");
	printf("�������� ��� ��'� (���������� %d � 28 �������): ", 28 - nameLength);
	std::cout << (randomNickname) << " " << (name);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// ������������ ��� ��� Index ��� ������
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

// ���� �������� �������� Alt ���� �� ��������� �����
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
			else {  // ��������� �������
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

// ���� ����� ������� �� � ������� ����. ����� ���� ���� ��� ����

// ���� ��������� �� ���� ��� � ������
void GameSystem::printRangeOfSavings(const std::vector<SaveFile>& savingFiles) {
	unsigned int i = 0, j = 0, k = 0;
	std::string savingName;
	size_t lastSaving;
	std::vector<std::vector<Saving>> tenSavingsArray;

	// ��������� �� ����� �� 10 (������������ ������)
	size_t groupCount = (savingFiles.size() + 9) / 10;
	tenSavingsArray.resize(groupCount);

	// ����������� ����� ������� �� ��� ��� �������
	updateIndex(savingsIndex, tenSavingsArray.size() - 1);


	// ����� �� ������ �������� �� ��������� �� � ����� �� 10 
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

	// �����
	std::cout << "\x1B[2J\x1B[H";
	printf("���������� ESC, ��� ����������� �� ��������� ����.\n");
	printf("����������� ˳��� ��� ������ Alt �� ��������, �������� ������� � ������������.\n");

	lastSaving = tenSavingsArray[savingsIndex].size()-1;

	printf("³���������� ��������� %d - %d � %d:\n\n", tenSavingsArray[savingsIndex][0].count,
		tenSavingsArray[savingsIndex][lastSaving].count, savingFiles.size());

	printf("\t%-35s%s\n\n", "���������� ������", "���� ����������");


	// ���� ��������� ������� �������
	for (const auto& saving : tenSavingsArray[savingsIndex]) {
		std::ostringstream oss;
		oss << '\t' << saving.count << ". " << std::left << std::setw(32) << saving.savingName
			<< std::put_time(std::localtime(&saving.time_t), "%Y-%m-%d %H:%M") << std::endl;

		// �� ����� ����� ��������� ����� �������� ��� ������� ���������� �������� ������ ����������
		std::cout << ((saving.count % 2) ? Services::graphics().colorize(oss.str(), 30, 47) : oss.str());
		//std::cout << oss.str();
	}
	printf("\n\n������ ����������: ");
}


bool GameSystem::savingHasBeenSelected(const std::string& folderPath, const std::string& extension) {
	unsigned int i = 0;
	std::vector<SaveFile> savingFiles;

	// ����� ��������� � �����
	for (const auto& entry : fs::directory_iterator(folderPath)) {
		if (entry.is_regular_file() && entry.path().extension() == extension
			&& getFileName(entry).size() <= 28) {
			auto lastModifiedTime = fs::last_write_time(entry);
			savingFiles.push_back({ entry, lastModifiedTime });
		}
	}

	// ���������� ������ �� ����� BUBBLE SORT
	for (int i = 0; i < savingFiles.size() - 1; i++) {
		for (int j = 0; j < savingFiles.size() - i - 1; j++) {
			if (savingFiles[j].time < savingFiles[j + 1].time) {
				std::swap(savingFiles[j], savingFiles[j + 1]);
			}
		}
	}

	// ������������� ���� ���������
	printRangeOfSavings(savingFiles);


	int choice;
	std::string input;
	while (true) {
		choice = 0;
		input = "";		

		// ������ �������� �����������
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
					return false; // ����������� �� ������� ����.
				}
				else if (key == '\r') { // Enter
					if (!input.empty()) {
						try {
							choice = std::stoi(input);
							break; // ����� � ����� ��������
						}
						catch (const std::exception&) {
							std::cout << "\n������������ ������ �����. ��������� �����: ";
							input = "";
						}
					}
				}
				else if (key == '\b' && !input.empty()) { // Backspace
					input.pop_back();
					std::cout << "\b \b"; // ������� ������ �� ������
				}
				else if (key >= '0' && key <= '9') { // �����
					input += key;
					std::cout << (char)key; // �������� ������� �����
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		choice--;
		if (choice < 0 || choice > savingFiles.size()-1)
		{
			std::cout << "\x1B[2J\x1B[H";
			printf("�� ����� ������������ ����� ����������. ��� ���������� ����� ��������� Enter...\n");
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
	// �������� �� ��������� �����
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
		std::cerr << "�� ������� ������� ���� names.json" << std::endl;
	}

	std::stringstream buffer;
	buffer << namesFile.rdbuf();
	std::string jsonContent = buffer.str();

	// ������� JSON
	rapidjson::Document document;
	if (document.Parse(jsonContent.c_str()).HasParseError()) {
		std::cerr << "������� �������� JSON" << std::endl;
	}

	// ����������, �� �� �����
	if (!document.IsArray()) {
		std::cerr << "JSON �� � �������" << std::endl;
	}

	// �������� �� �������� ������

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
	// ����, �� �� �� ������������
	getPlayerNicknames();

	while (true) {
		// restart randomizer
		srand(static_cast<unsigned>(time(nullptr)));

		nicknameIndex = rand() % nicknames.size();
		randomNickname = nicknames[nicknameIndex]; // ������ ����������� �������� ��������� �� ������� ���
		nameLength = name.length() + randomNickname.length() + 1;

		displayPrompt();
		if (!nicknamePicked()) return false;

		std::cout << "\n";
		Services::graphics().print("���� ��'�: ", 500);

		// ���� ��'� �� ���� ������� - ����������� ��� ���������.
		name += (!name.empty() ? " " + randomNickname : randomNickname);

		Services::graphics().print(name, 500);

		std::cout << "\n\n";
		Services::graphics().print("ϳ��������� ����? ", 100);
		std::cout << "(Enter/ESC)";

		// ������� �����
		while (_kbhit()) {
			_getch();
		}
		// ������� ���������� ������
		int key = _getch();

		if (key == '\r') { // Enter
			if (!hasFilesWithExtension("./assets/savings/", ".json", name)) {
				std::string filename = "./assets/savings/" + name + ".json";

				// ����� ����� � ����!!!!!
				writePlayerData(filename, randomNickname, 0, 1, 0, 0);
				return true;
			}
			else {
				std::cout << "\x1B[2J\x1B[H";
				printf("���� ���������� ��� ����. ��� ���������� ����� ��������� Enter...\n");
				_getch();
				// ������������? �������� �� ����� ����������
				name.clear();
			}
		}
		else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { // ESC
			name.clear();
			// ���������� ����
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

// ���� ��������� ����
void GameSystem::printMainMenu() {
	// �������� ����
	std::cout << "\x1B[2J\x1B[H";
	printf("1. ���������� ��������\n");
	printf("2. ���� ���\n");
	printf("3. �� ��������?\n");
	printf("4. ������ ��������\n");
	printf("0. �������� �����\n");

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

// ����� ��������� ����
void GameSystem::mainMenuLogic() {
	Services::graphics().SetWindowSize(145, 40);
	printMainMenu();

	while (true) {
		if (_kbhit()) {
			int key = _getch();

			switch (key) {

			case '\r':
			case '1':   // ������ �1�
				if (!hasFilesWithExtension("./assets/savings", ".json"))
				{
					std::cout << "\x1B[2J\x1B[H";
					std::cout << "��������� �� ��������. ��� ����������� �� ��������� ���� ��������� ����-��� ������...\n";
					_getch();
					printMainMenu();
				}

				else if (savingHasBeenSelected("./assets/savings", ".json"))
					return; // ����� ���� ������ ����������
				else printMainMenu();

				break;

			case '2':   // ������ �2�
				// ����� � �������, ���� newGame() ��������� true
				if (newGame())
					return;
				else printMainMenu();

				break;

			case '3':   // ������ �3�
				std::cout << "\x1B[2J\x1B[H";
				std::cout << "����� ����������� � ��������...\n";
				std::cout << "ϳ������� ���������� ����� =)\n\n";
				std::cout << "��� ���������� ��������� Enter...\n";
				_getch();
				printMainMenu();
				break;

			case '4':{   // ������ �4�
				std::string cmd = "start \"\" \"assets/settings/See more content webpage/more.html\"";
				int ret = std::system(cmd.c_str());

				if (ret != 0) {
					std::cerr << "�� ������� ������� ��������, ���: " << ret << "\n";
					std::exit(1);
				}
			}
				break;

			case '0':   // ������ �0�
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
				std::cout << "Au revoir, ����������� ����!\n";

				Sleep(12000);
				std::exit(0);
				return;
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


// TODO: �������� �� ���� ���� �� � � ����� �����
std::string GameSystem::getMusicFiles(const std::string& folderPath, const std::string& extension) {
	std::vector<std::string> arrayOfFiles;

	// ����� ��������� ����� � �����
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
		printf("������ ���������� �� ���� ��� � �� ������� �� ���� (Enter ��� ����������)...\n");
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

		// ���������� � �������. ��������� ��������� ����. Frontend
		mainMenuLogic();


		// ����� ������ ����� ���� � ���� � ���� ��������� �� ������
		std::cout << "\x1B[2J\x1B[H";

		// � �� ����, �� �� �� ������������!
		Services::graphics().print("�������� �� �� ����� ������������, ", 0);
		Services::graphics().print(name, 1000);
		//Services::graphics().print("...", 1500, 3000);

		Services::graphics().unprint("�������� �� �� ����� ������������, " + name + "...\b", 100);
		std::string filename = "./assets/savings/" + name + ".json";
		printf("%d ������������ ������� �� �� ��'�.", readPlayerDeaths(filename));
		//Sleep(3000);

		// ����������� �����, �������� ������� ����� ���� ������������� � ���. Backend

		// ³������� ��� ��������� ����� (��� �������� ����)

		// ³������� Settings json, ��������� ������
		std::ifstream ifs("assets/settings/settings.json");
		if (!ifs.is_open()) {
			std::cerr << "�� ������� ������� settings.json\n";
		}
		rapidjson::IStreamWrapper isw(ifs);
		rapidjson::Document doc;
		doc.ParseStream(isw);
		if (doc.HasParseError() || !doc.IsObject()) {
			std::cerr << "������� ������ settings.json\n";
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
		_player.init(1, 10, 100, 10, readPlayerMoney(filename)); // ������� ���������� ��������� �������!


		std::cout << "\x1B[2J\x1B[H";
		loadLevel(filename, musicName, _player);


		Graphics graphics;
		graphics.init();

		//Sleep(10000);

		std::thread missile_thread(missile_thread_func);
		std::thread cannon_thread(cannon_thread_func);
		std::thread enemy_thread(enemy_thread_func);
		std::thread draw(draw_thread);


		// ��� ���� �������� ��������� ���� ����� ��!
		while (!isDone) // isBadEnd?
		{
			char key = _level.Move(_getch(), _player);

			if (key == 'r' || key == 'R') {
				// --- ������� ���� ---
				PauseTheGame();
				PauseDrawThread();
				Services::message().clearMessageList();
				std::cout << "\x1B[2J\x1B[H";
				//printf("���������� ����...\n");
				_level.clear();
				graphics.setCursorPos(0, 0);
				graphics.init();

				_level.setPlayerName(name);
				_player.init(1, 10, 100, 10, readPlayerMoney(filename)); // ������� ���������� ��������� �������!
				loadLevel(filename, musicName, _player);
				UnPauseDrawThread();
				UnPauseTheGame();
				continue;
			}

			// �������� ��� ��������� ������������ �� CPU
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		system("cls");
		printf("You win!!! ������� �� ��������� ���� (4s)...");

		// ����� ����� � ����!!!!!
		int playerLevel = readPlayerLevel(filename);
		writePlayerData(filename, readPlayerName(filename), 0, ++playerLevel, _player.GetAvailableMoney(), readPlayerDeaths(filename));
		// 0 � ���� �� ������ "�������" ������� �� �������� �����.

		mciSendStringA("close leMusic", NULL, 0, NULL);
		// 
		// ���������� ���������� ������ � ����� ������
		missile_thread.join();
		cannon_thread.join();
		enemy_thread.join();
		draw.join();

		// ��������: ������������ �������� ��� ���������� ������
		isDone = true;
		isBadEnd = true; // ��� ����� �����������



		Sleep(4000);
		system("cls");
	}
}
