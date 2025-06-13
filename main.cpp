#include "GameSystem.h"
#include <windows.h>

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	
	GameSystem gameSys;

	gameSys.RunGame();

	return 0;
}