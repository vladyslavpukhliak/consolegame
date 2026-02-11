#include "Message.h"
#include "Level.h"
#include "Graphics.h"
#include "GameSystem.h"
#include "constants.h"
#include "Services.h"

#include <iostream>

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

// �� �� �� �������� � chrono, �� ��� ��� �� ��������?
void Message::checkExpiredmessageList() {
	std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

	Message frontMessage;
	std::chrono::duration<double> elapsedTime;

	while (!messageQueue.empty()) {
		frontMessage = messageQueue.front();

		elapsedTime = now - frontMessage.timestamp;

		if (GameSystem::isGameOver()) break;
		if (elapsedTime.count() >= MESSAGE_DISPLAY_TIME || messageQueue.size() >= MAX_MESSAGES_DISPLAYED) {
			clearRow();
			printmessageList();
		}
		else {
			break;  // ����� � �����, ���� ����� ��������� �� ���������
		}
	}
}


// ������� ��������� �����������
void Message::clearRow() {
	if (s_busy) return;
	s_busy = true;
	std::queue<Message> tempQueue = messageQueue;  // ������� ����� �� ����������� ����������
	int i = 1;
	size_t messageLength;


	while (!tempQueue.empty()) {
		//if (Level::isBusy()) return; // ���� ���� �� ��������, �� ������� �� ��������� �����:
		messageLength = tempQueue.front().text.size();
		std::string spaces(messageLength, ' ');

		Services::graphics().setCursorPos(MESSAGES_INIT_POS, i);
		printf(spaces.c_str());
		Services::graphics().setCursorPos(0, 0);
		tempQueue.pop();
		i++;
	}
	messageQueue.pop();
	s_busy = false;
}

void Message::printmessageList() {
	if (s_busy) return;
	s_busy = true;
	std::queue<Message> tempQueue = messageQueue;  // ������� ����� �� ����������� ����������
	int i = 1;
	std::string frontMessage;

	while (!tempQueue.empty()) {
		frontMessage = tempQueue.front().text;
		Services::graphics().setCursorPos(MESSAGES_INIT_POS, i);
		std::cout << frontMessage;
		//printf(frontMessage.c_str());
		Services::graphics().setCursorPos(0, 0);
		tempQueue.pop();
		i++;
	}
	s_busy = false;
}