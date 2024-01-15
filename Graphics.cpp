#include "Graphics.h"
#include "Message.h"
#include <Windows.h>

Message messagesManager;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD cursor_pos;

void Graphics::setCursorPos(short x, short y) {
	cursor_pos = { x, y };
	SetConsoleCursorPosition(console, cursor_pos);
}

void SetWindowSize(int width, int height) {
	COORD size = {width, height};
	SMALL_RECT rmin = {0,0,1,1};
	SMALL_RECT rect = {0,0,width-1,height-1};
	SetConsoleWindowInfo(console, TRUE, &rmin);
	SetConsoleScreenBufferSize(console, size);
	SetConsoleWindowInfo(console, TRUE, &rect);
}

void Graphics::init() {
	SetConsoleTitleA("Demo");

	// Відключення курсора
	CONSOLE_CURSOR_INFO cursor_info;
	GetConsoleCursorInfo(console, &cursor_info);
	cursor_info.bVisible = false;
	SetConsoleCursorInfo(console, &cursor_info);

	// Задаємо розмір вікна гри
	SetWindowSize(100, 30); // change digits to json values

	// Ініціалізуємо фіксований UI
	setCursorPos(40, 0);
	printf("-- Information --");
}

void Graphics::addMessage(string message) {
	messagesManager.addMessage(message);
}