#include "Graphics.h"
#include "Message.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <sstream>
#include <windows.h>

Message messageListManager;
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
COORD cursor_pos;

void getScreenBufferInfo() {
	GetConsoleScreenBufferInfo(console, &bufferInfo);
}
short Graphics::GetCurX() {
	getScreenBufferInfo();
	return bufferInfo.dwCursorPosition.X;
}
short Graphics::GetCurY() {
	getScreenBufferInfo();
	return bufferInfo.dwCursorPosition.Y;
}

void Graphics::setCursorPos(short x, short y) {
	cursor_pos = { x, y };
	SetConsoleCursorPosition(console, cursor_pos);
}

void SetWindowSize(int width, int height) {
	COORD size = { width, height };
	SMALL_RECT rmin = { 0,0,1,1 };
	SMALL_RECT rect = { 0,0,width - 1,height - 1 };
	SetConsoleWindowInfo(console, TRUE, &rmin);
	SetConsoleScreenBufferSize(console, size);
	SetConsoleWindowInfo(console, TRUE, &rect);
}

void Graphics::init() {
	SetConsoleTitleA("Demo");

	// Відключення курсора
	CONSOLE_CURSOR_INFO cursor_info;
	GetConsoleCursorInfo(console, &cursor_info);
	cursor_info.bVisible = false;
	SetConsoleCursorInfo(console, &cursor_info);

	// Задаємо розмір вікна гри
	SetWindowSize(100, 30); // change digits to json values

	// Ініціалізуємо фіксований UI
	setCursorPos(40, 0);
	printf("-- Information --");
}

void Graphics::addMessage(std::string message) {
	messageListManager.addMessage(message);
}

std::vector<std::string> splitString(const std::string& str) {
	std::vector<std::string> words;
	std::istringstream iss(str);
	std::string word;

	while (iss >> word) {
		words.push_back(word);
	}

	return words;
}

std::string Graphics::colorizeTile(const char& tile, const unsigned int& fg, unsigned int bg) {
	return "\033[" + std::to_string(fg) + ";" + std::to_string(bg) + "m" + tile + "\033[0m";
}

void Graphics::print(const std::string &str, const unsigned int miliseconds, const unsigned int milisecondsSpeed) {
	std::vector<std::string> words = splitString(str);

	for (const auto& word : words) {
		for (size_t i = 0; i < word.length(); i++)
		{
			std::cout << word[i];
			std::this_thread::sleep_for(std::chrono::milliseconds(milisecondsSpeed / word.length()));
		}
		std::cout << " ";
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
}


// шось воно не все стирає. виправ
void Graphics::unprint(const std::string &str, const unsigned int miliseconds) {
	std::vector<std::string> words = splitString(str);

	for (const auto& word : words) {
		for (size_t i = 0; i < word.length(); i++)
		{
			std::cout << "\b \b";
			std::this_thread::sleep_for(std::chrono::milliseconds(80 / word.length()));
		}
		//std::cout << " ";
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
}