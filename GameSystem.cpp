#include <Windows.h>

#include "GameSystem.h"
#include <conio.h>
#include <ctime>

using namespace std;
// Constructor sets up the game
GameSystem::GameSystem(string levelFile) {

	_player.init(1, 10, 100, 10);

	_level.load(levelFile, _player);

	Sleep(600);
}

void GameSystem::RunGame() {

	bool isDone = false;
	PlaySoundA("moon_crystals.wav", NULL, SND_ASYNC);

	while (!isDone)
	{
		_level.print();
		PlayerInput();
		_level.UpdateEnemies(_player);
	}

}

void GameSystem::PlayerInput() {

	char input = get_char(.5);
	if(input != '_')
		_level.Move(input, _player);

}

char GameSystem::get_char(float time_limit) {
	time_t start = time(NULL); //start the clock 
	while (!_kbhit()) {
		/*compare the clock with current time, if it exceeds return -1;*/
		if ((time(NULL) - start) >= time_limit)
			return '_';
	}
	//else return the value stored in keyboard buffer. 
	return _getch();
}
