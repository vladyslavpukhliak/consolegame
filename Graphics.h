#pragma once
#include <string>

class Graphics
{
public:
	void init();

/// <summary>
/// ��������� ���� ������ ������
/// </summary>
/// <param name="str">����� ������</param>
/// <param name="miliseconds">������ �������� ����� ����� ���� ������</param>
	void print(const std::string& str, const unsigned int miliseconds, const unsigned int milisecondsSpeed = 80);
	void unprint(const std::string& str, const unsigned int miliseconds);
	std::string colorizeTile(const char& tile, const unsigned int& fg, unsigned int bg = 40);

	void setCursorPos(short x, short y);
	void addMessage(std::string message);
	short GetCurX();
	short GetCurY();
};

