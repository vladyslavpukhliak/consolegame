#include "Message.h"
#include "Graphics.h"
using namespace std;

Graphics graphics;
queue<Message> messageQueue;

void Message::addMessage(const string text) {
	Message newMessage(text);
	messageQueue.push(newMessage);
	printMessages();
}

void Message::checkExpiredMessages() {
	//if (Level::isBusy()) return;

	chrono::time_point<chrono::steady_clock> now = chrono::steady_clock::now();

	Message frontMessage;
	chrono::duration<double> elapsedTime;

	while (!messageQueue.empty()) {
		frontMessage = messageQueue.front();

		elapsedTime = now - frontMessage.timestamp;

		if (elapsedTime.count() >= 1) {  // Час, через який сповіщення зникає (1000 мілісекунд)
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
	queue<Message> tempQueue = messageQueue;  // Копіюємо чергу до тимчасового контейнера
	int i = 1;
	size_t messageLength;


	while (!tempQueue.empty()) {
		//if (Level::isBusy()) return; // Якщо друк не зайнятий, то перейти до наступних кроків:
		messageLength = tempQueue.front().text.size();
		string spaces(messageLength, ' ');

		graphics.setCursorPos(40, i);
		printf(spaces.c_str());
		tempQueue.pop();
		i++;
	}
	messageQueue.pop();
}

void Message::printMessages() {
	queue<Message> tempQueue = messageQueue;  // Копіюємо чергу до тимчасового контейнера
	int i = 1;
	string frontMessage;

	while (!tempQueue.empty()) {
		//if (Level::isBusy()) return;
		frontMessage = tempQueue.front().text;
		graphics.setCursorPos(40, i);
		printf(frontMessage.c_str());
		tempQueue.pop();
		i++;
	}
}

