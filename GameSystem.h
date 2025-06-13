#pragma once
#include "Level.h"
#include "Player.h"
#include "Graphics.h"
#include <string>

class GameSystem
{
public:
	//GameSystem(std::string levelFile);

	void RunGame();
	static void BadEnding();
	static bool isGameOver();

private:
	Graphics graphicsManager;
	std::string name, randomNickname;
	std::vector<std::string> nicknames;
	uint8_t nameLength = 0;
	int nicknameIndex = 0;

	bool newGame();
	void mainMenuLogic();
	void getPlayerNicknames();
	bool savingHasBeenSelected(const std::string& folderPath, const std::string& extension);

	bool nicknamePicked();
	void updateNicknameIndex();
	void displayPrompt();
	void writePlayerData(const std::string& filename, const std::string& name, int exp, int level, double money);
};
