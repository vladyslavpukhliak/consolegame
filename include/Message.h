#pragma once
#include <chrono>
#include <string>
#include <queue>

class Message {
public:
    std::string text;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    Message() = default;
    Message(const std::string txt) : text(txt), timestamp(std::chrono::steady_clock::now()) {}

public:
    void addMessage(const std::string text);
    void checkExpiredmessageList();
    void printmessageList();
    static bool isBusy();

private:
    void clearRow();
};