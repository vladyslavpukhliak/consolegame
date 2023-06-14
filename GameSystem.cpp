#include <Windows.h>
#include <cstdlib>
#include <conio.h>
#include <thread>
#include <chrono>
#include "GameSystem.h"

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
		_level.print();
		// Зупинка потоку на 500 мілісекунд
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void initialise() {
	// Відключення курсора
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor_info;
	GetConsoleCursorInfo(console, &cursor_info);
	cursor_info.bVisible = false;
	SetConsoleCursorInfo(console, &cursor_info);

	// Отримання розміру консольного вікна
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	GetConsoleScreenBufferInfo(console, &buffer_info);
	int console_width = buffer_info.srWindow.Right - buffer_info.srWindow.Left + 1;
	int console_height = buffer_info.srWindow.Bottom - buffer_info.srWindow.Top + 1;

	// Встановлення режиму буферизованого вводу/виводу
	DWORD console_mode;
	GetConsoleMode(console, &console_mode);
	console_mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
	console_mode |= ENABLE_EXTENDED_FLAGS;
	SetConsoleMode(console, console_mode);
}

void GameSystem::RunGame() {
	initialise();
	
	mciSendStringA("open \"Music/moon_crystals.mp3\" type mpegvideo alias level1.mp3", NULL, 0, NULL);
	mciSendStringA("play level1.mp3", NULL, 0, NULL);

	std::thread enemy_thread(enemy_thread_func_wrapper, this);

	while (!isDone)
	{
		_level.Move(_getch(), _player);		
		_level.print();
		// Затримка головного потоку на 16 мілісекунд (приблизно 60 кадрів в секунду)
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
	system("cls");
	printf("You win!!!");
	mciSendStringA("close level1.mp3", NULL, 0, NULL);

	// Очікування завершення потоку з рухом ворогів
	enemy_thread.join();
}
