#include "Message.h"
#include "Level.h"
#include "Graphics.h"
using namespace std;

Graphics graphics;
queue<Message> messageQueue;
bool s_busy = false;

void Message::addMessage(const string text) {
	Message newMessage(text);
	messageQueue.push(newMessage);
	printMessages();
}

bool Message::isBusy() { return s_busy; };

// Шо це за навороти з chrono, які бог зна як працюють?
void Message::checkExpiredMessages() {
	chrono::time_point<chrono::steady_clock> now = chrono::steady_clock::now();

	Message frontMessage;
	chrono::duration<double> elapsedTime;

	while (!messageQueue.empty()) {
		frontMessage = messageQueue.front();

		elapsedTime = now - frontMessage.timestamp;

		//if (Level::isBusy()) return;
		if (elapsedTime.count() >= 1 || messageQueue.size() >= 15) {  // Час, через який сповіщення зникає (1000 мілісекунд)
			clearRow();
			printMessages();
		}
		else {
			break;  // Вийти з циклу, якщо перше сповіщення ще актуальне
		}
	}
}


// Очищуємо попередні повідомлення
void Message::clearRow() {
	if (s_busy) return;
	s_busy = true;
	queue<Message> tempQueue = messageQueue;  // Копіюємо чергу до тимчасового контейнера
	int i = 1;
	size_t messageLength;


	while (!tempQueue.empty()) {
		//if (Level::isBusy()) return; // Якщо друк не зайнятий, то перейти до наступних кроків:
		messageLength = tempQueue.front().text.size();
		string spaces(messageLength, ' ');

		graphics.setCursorPos(40, i);
		printf(spaces.c_str());
		graphics.setCursorPos(0, 0);
		tempQueue.pop();
		i++;
	}
	messageQueue.pop();
	s_busy = false;
}

void Message::printMessages() {
	if (s_busy) return;
	s_busy = true;
	queue<Message> tempQueue = messageQueue;  // Копіюємо чергу до тимчасового контейнера
	int i = 1;
	string frontMessage;

	while (!tempQueue.empty()) {
		frontMessage = tempQueue.front().text;
		graphics.setCursorPos(40, i);
		printf(frontMessage.c_str());
		graphics.setCursorPos(0, 0);
		tempQueue.pop();
		i++;
	}
	s_busy = false;
}
