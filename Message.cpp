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
	chrono::time_point<chrono::steady_clock> now = chrono::steady_clock::now();

	while (!messageQueue.empty()) {
		Message frontMessage = messageQueue.front();

		chrono::duration<double> elapsedTime = now - frontMessage.timestamp;
		if (elapsedTime.count() >= 1) {  // ���, ����� ���� ��������� ����� (1000 ��������)
			clearRow();
			messageQueue.pop();
			printMessages();
		}
		else {
			break;  // ����� � �����, ���� ����� ��������� �� ���������
		}
	}
}


// ������� �������� �����������
void Message::clearRow() {
	queue<Message> tempQueue = messageQueue;  // ������� ����� �� ����������� ����������
	int i = 1;

	while (!tempQueue.empty()) {
		size_t messageLength = tempQueue.front().text.size();
		graphics.setCursorPos(40, i);
		for (int j = 0; j < messageLength; j++) {
			printf(" ");
		}
		tempQueue.pop();
		i++;
	}
}

void Message::printMessages() {
	queue<Message> tempQueue = messageQueue;  // ������� ����� �� ����������� ����������
	int i = 1;

	while (!tempQueue.empty()) {
		string frontMessage = tempQueue.front().text;
		graphics.setCursorPos(40, i);
		printf(frontMessage.c_str());
		tempQueue.pop();
		i++;
	}
}

