#include "Message.h"
#include "Level.h"
#include "Graphics.h"
#include "GameSystem.h"
#include "constants.h"
#include <iostream>

Graphics graphics;
std::queue<Message> messageQueue;
bool s_busy = false;
size_t currentMessagePos = 0;
void Message::clearMessageList() {
	messageQueue = {};
}

void Message::addMessage(const std::string text) {

	if (text.length() > MAX_MESSAGE_LENGTH)
	{
		currentMessagePos = 0;
		while (currentMessagePos < text.length())
		{
			std::string segment = text.substr(currentMessagePos, MAX_MESSAGE_LENGTH);
			currentMessagePos += MAX_MESSAGE_LENGTH;
			
			Message newMessage(segment);
			messageQueue.push(newMessage);
		}
	}
	else {
		Message newMessage(text);
		messageQueue.push(newMessage);
	}
	//printmessageList();
}

bool Message::isBusy() { return s_busy; };

// Шо це за навороти з chrono, які бог зна як працюють?
void Message::checkExpiredmessageList() {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	Message frontMessage;
	std::chrono::duration<double> elapsedTime;

	while (!messageQueue.empty()) {
		frontMessage = messageQueue.front();

		elapsedTime = now - frontMessage.timestamp;

		if (GameSystem::isGameOver()) break;
		if (elapsedTime.count() >= 3.5 || messageQueue.size() >= 15) {  //    ,                            (1000         )
			clearRow();
			printmessageList();
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
	std::queue<Message> tempQueue = messageQueue;  // Копіюємо чергу до тимчасового контейнера
	int i = 1;
	size_t messageLength;


	while (!tempQueue.empty()) {
		//if (Level::isBusy()) return; // Якщо друк не зайнятий, то перейти до наступних кроків:
		messageLength = tempQueue.front().text.size();
		std::string spaces(messageLength, ' ');

		graphics.setCursorPos(MESSAGES_INIT_POS, i);
		printf(spaces.c_str());
		graphics.setCursorPos(0, 0);
		tempQueue.pop();
		i++;
	}
	messageQueue.pop();
	s_busy = false;
}

void Message::printmessageList() {
	if (s_busy) return;
	s_busy = true;
	std::queue<Message> tempQueue = messageQueue;  // Копіюємо чергу до тимчасового контейнера
	int i = 1;
	std::string frontMessage;

	while (!tempQueue.empty()) {
		frontMessage = tempQueue.front().text;
		graphics.setCursorPos(MESSAGES_INIT_POS, i);
		std::cout << frontMessage;
		//printf(frontMessage.c_str());
		graphics.setCursorPos(0, 0);
		tempQueue.pop();
		i++;
	}
	s_busy = false;
}