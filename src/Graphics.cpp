#include "Graphics.h"
#include "Message.h"
#include "constants.h"
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

void Graphics::SetWindowSize(int width, int height) {
	COORD size = { width, height };
	SMALL_RECT rmin = { 0,0,1,1 };
	SMALL_RECT rect = { 0,0,width - 1,height - 1 };
	SetConsoleWindowInfo(console, TRUE, &rmin);
	SetConsoleScreenBufferSize(console, size);
	SetConsoleWindowInfo(console, TRUE, &rect);
}

void Graphics::SetWindowTitle(std::string title) {
	SetConsoleTitleA(title.c_str());
}

void Graphics::init() {
	SetWindowTitle(TITLE + " | Demo");

	// Відключення курсора
	CONSOLE_CURSOR_INFO cursor_info;
	GetConsoleCursorInfo(console, &cursor_info);
	cursor_info.bVisible = false;
	SetConsoleCursorInfo(console, &cursor_info);

	// Задаємо розмір вікна гри
	SetWindowSize(120, 60); // change digits to json values

	// Ініціалізуємо фіксований UI
	setCursorPos(40, 0);
	printf("-- Information --");
}

// Жорстка допомога ШІ з конвертацією зроз, я втомився
// Конвертація UTF-8 рядка в ANSI
std::string Graphics::Utf8ToAnsi(const std::string& utf8)
{
	if (utf8.empty()) return {};

	// UTF-8 → UTF-16
	int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
	if (wlen == 0) return {};
	std::wstring wstr(wlen, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wstr[0], wlen);
	if (!wstr.empty() && wstr.back() == L'\0') wstr.pop_back();

	// UTF-16 → ANSI (системна кодова сторінка)
	int alen = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (alen == 0) return {};
	std::string ansi(alen, '\0');
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, &ansi[0], alen, nullptr, nullptr);
	if (!ansi.empty() && ansi.back() == '\0') ansi.pop_back();

	return ansi;
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
		std::cout << "\b \b";
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(miliseconds));
}