#pragma once
#include <string>
using namespace std;
class Graphics
{
public:
	void init();
	void setCursorPos(short x, short y);
	void addMessage(string message);
};

