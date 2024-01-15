#pragma once
#include <chrono>
#include <string>
#include <queue>
using namespace std;

class Message {
public:
    string text;
    chrono::time_point<chrono::steady_clock> timestamp;
    Message() = default;
    Message(const string txt) : text(txt), timestamp(chrono::steady_clock::now()) {}

public:
    void addMessage(const string text);
    void checkExpiredMessages();
    void printMessages();
    static bool isBusy();

private:
    void clearRow();
};