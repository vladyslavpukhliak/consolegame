#include <Windows.h>
#pragma comment(lib, "winmm.lib")
#include <cstdlib>
#include <conio.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include "GameSystem.h"
#include "Graphics.h"

bool isDone = false;

// Constructor sets up the game
GameSystem::GameSystem(string levelFile) {

	_player.init(1, 10, 100, 10);

	_level.load(levelFile, _player);
}

void enemy_thread_func_wrapper(GameSystem* game_system)
{
	game_system->enemy_thread_func();
}

void GameSystem::enemy_thread_func()
{
	while (!isDone)
	{
		
		if (_level.buttonPlate == 0) isDone = true;
		// Оновлення позицій ворогів
		_level.UpdateEnemies(_player);
		_level.Draw();

		// Зупинка потоку на 500 мілісекунд
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}


void GameSystem::RunGame() {
	/*
	string name;
	printf("Enter your nickname: ");
	getline(cin, name);
	system("cls");
	*/

	Graphics graphics;
	graphics.init();
	
	
	mciSendStringA("open \"Music/moon_crystals.mp3\" type mpegvideo alias level1.mp3", NULL, 0, NULL);
	//mciSendStringA("play level1.mp3", NULL, 0, NULL);

	std::thread enemy_thread(enemy_thread_func_wrapper, this);

	while (!isDone)
	{
		
		_level.Move(_getch(), _player);		
		_level.Draw();
		// Затримка головного потоку на 16 мілісекунд (приблизно 60 кадрів в секунду)
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
	system("cls");
	printf("You win!!!");
	mciSendStringA("close level1.mp3", NULL, 0, NULL);

	// Очікування завершення потоку з рухом ворогів
	enemy_thread.join();
}
