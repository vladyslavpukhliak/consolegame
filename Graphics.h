#pragma once
#include <string>

class Graphics
{
public:
	void init();
	void setCursorPos(short x, short y);
	void addMessage(std::string message);
};

