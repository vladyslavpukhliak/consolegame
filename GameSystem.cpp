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
#include "Message.h"

Level _level;
Message messages;
Player _player;
bool isDone = false;

// Constructor sets up the game
GameSystem::GameSystem(string levelFile) {

	_player.init(1, 10, 100, 10);

	_level.load(levelFile, _player);
}

void enemy_thread_func()
{
	while (!isDone)
	{
		//if(!Message::isBusy) 
		// ��������� ������� ������
		//if (Level::isBusy) {
			_level.UpdateEnemies(_player);
		//}

		// ������� ������ �� 500 ��������
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
void draw_thread() {
	while (!isDone)
	{
		if (_level.buttonPlate == 0) isDone = true;
		messages.checkExpiredMessages();
		_level.Draw();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


void GameSystem::RunGame() {
	
	string name;
	printf("Enter your nickname: ");
	getline(cin, name);
	_level.setPlayerName(name);
	system("cls");
	

	Graphics graphics;
	graphics.init();
	
	
	mciSendStringA("open \"Music/moon_crystals.mp3\" type mpegvideo alias level1.mp3", NULL, 0, NULL);
	//mciSendStringA("play level1.mp3", NULL, 0, NULL);

	std::thread enemy_thread(enemy_thread_func);
	std::thread draw(draw_thread);

	while (!isDone)
	{
		//if (!Level::isBusy()) {
			_level.Move(_getch(), _player);
			//_level.Draw();
		//}
		// �������� ��������� ������ �� 16 �������� (��������� 60 ����� � �������)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	system("cls");
	printf("You win!!!");
	mciSendStringA("close level1.mp3", NULL, 0, NULL);

	// ���������� ���������� ������ � ����� ������
	enemy_thread.join();
}
