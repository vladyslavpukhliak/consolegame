#pragma once
#include "Level.h"
#include "Player.h"
#include "Graphics.h"
#include <string>
#include <filesystem>
namespace fs = std::filesystem;


class GameSystem
{
public:
	//GameSystem(std::string levelFile);

	void RunGame();
	static void BadEnding();
	static bool isGameOver();
	static void PauseTheGame();
	static void UnPauseTheGame();
	void saveAfterDeath(std::string& savingName);

private:
	Graphics graphicsManager;
	std::string name, randomNickname, artToPrint = initMainArt("assets/Art/Header.txt"), qrToPrint = initMainArt("assets/Art/qr.txt");
	std::vector<std::string> nicknames;
	uint8_t nameLength = 0;
	int nicknameIndex = 0, savingsIndex = 0;

	struct SaveFile {
		fs::directory_entry entry;
		fs::file_time_type time;
	};
	struct Saving {
		unsigned int count = 0;
		std::string savingName;
		time_t time_t;
	};

	bool newGame();
	void mainMenuLogic();
	void getPlayerNicknames();
	bool savingHasBeenSelected(const std::string& folderPath, const std::string& extension);

	bool nicknamePicked();
	void updateIndex(int& currentIndex, const unsigned int lastIndex);
	void updateNickname();

	std::string initMainArt(const std::string& path);
	void printMainMenu();

	void printRangeOfSavings(const std::vector<SaveFile>& savingFiles);

	void displayPrompt();
	void writePlayerData(const std::string& filename, const std::string& name, int exp, int level,
		int money, int deaths);
};
