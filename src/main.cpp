#include "GameSystem.h"
#include <windows.h>
#include <iostream>

void enableANSI() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
}

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	/*SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);*/

	enableANSI();
	GameSystem gameSys;

	gameSys.RunGame();

	return 0;
}