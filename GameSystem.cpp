#pragma comment(lib, "winmm.lib")
#include <cstdlib>
#include <conio.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include "GameSystem.h"
#include "Graphics.h"
#include "Message.h"
#include <windows.h>

Level _level;
Message messageList;
Player _player;
bool isDone = false;
bool isBadEnd = false;

bool GameSystem::isGameOver() { return isBadEnd; }
void GameSystem::BadEnding() { isBadEnd = true; }
// Constructor sets up the game
GameSystem::GameSystem(std::string levelFile) {

	_player.init(1, 10, 100, 10);

	_level.load(levelFile, _player);
}

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
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


void GameSystem::RunGame() {
	
	std::string name;
	printf("Enter your nickname (up to 28 symbols): ");
	getline(std::cin, name);
	_level.setPlayerName(name);
	system("cls");
	

	Graphics graphics;
	graphics.init();
	
	
	mciSendStringA("open \"Music/moon_crystals.mp3\" type mpegvideo alias level1.mp3", NULL, 0, NULL);
	mciSendStringA("play level1.mp3", NULL, 0, NULL);

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
	mciSendStringA("close level1.mp3", NULL, 0, NULL);

	// Очікування завершення потоку з рухом ворогів
	enemy_thread.join();
}
