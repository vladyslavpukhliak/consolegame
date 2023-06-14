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

void Graphics::init() {
	// Відключення курсора
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

	setCursorPos(40, 0);
	printf("-- Information --");
}

void Graphics::addMessage(string message) {
	messagesManager.addMessage(message);
}