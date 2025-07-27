#include "GameSystem.h"
#include <windows.h>

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
	
	enableANSI();
	GameSystem gameSys;

	gameSys.RunGame();

	return 0;
}