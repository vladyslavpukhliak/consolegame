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
        if (elapsedTime.count() >= 0.6) {  // ���, ����� ���� ��������� ����� (600 ��������)
            clearRow(frontMessage.text.c_str());
        }
        else {
            break;  // ����� � �����, ���� ����� ��������� �� ���������
        }
    }
}

// ������� �������� �����������
void Message::clearRow(string toClear) {
    Message frontMessage = messageQueue.front();
    graphics.setCursorPos(40, messageQueue.size());
    messageQueue.pop();

    for (int j = 0; j < toClear.size(); j++)
        printf(" ");
}

void Message::printMessages() {
    queue<Message> tempQueue = messageQueue;  // ������� ����� �� ����������� ����������
    int i = 1;

    while (!tempQueue.empty()) {
        Message frontMessage = tempQueue.front();
        graphics.setCursorPos(40, i);
        printf(frontMessage.text.c_str());
        tempQueue.pop();
        i++;
    }
}


