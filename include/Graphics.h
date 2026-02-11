#pragma once
#include <string>

class Graphics
{
public:
	void init();
	std::string Utf8ToAnsi(const std::string& utf8);

/// <summary>
/// ���������� ���� ������ ������
/// </summary>
/// <param name="str">����� ������</param>
/// <param name="miliseconds">������ �������� ����� ����� ���� ������</param>
	void print(const std::string& str, const unsigned int miliseconds, const unsigned int milisecondsSpeed = 80);
	void unprint(const std::string& str, const unsigned int miliseconds);
	std::string colorize(const std::string& tile, unsigned int fg, unsigned int bg = 40) const {
		return "\033[" + std::to_string(fg) + ";" + std::to_string(bg) + "m" + tile + "\033[0m";
	};

	void setCursorPos(short x, short y);
	void addMessage(const std::string& message);
	void clearScreen();
	short GetCurX() const;
	short GetCurY() const;

	void SetWindowSize(int width, int height);
	void SetWindowTitle(const std::string& title);
};

