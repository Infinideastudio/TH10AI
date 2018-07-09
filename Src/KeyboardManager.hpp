#pragma once
#include "Vec2.hpp"
#include <Windows.h>
const char KEY_SHIFT = 16;
const char KEY_LEFT = 37;
const char KEY_UP = 38;
const char KEY_RIGHT = 39;
const char KEY_DOWN = 40;
const char KEY_X = 88;
const char KEY_Z = 90;
class KeyboardManager
{
public:
	static void init();
	static void sendKeyInfo(int dir,bool shift,bool z,bool x);
private:
	static bool keystate[100];
	static void sendKey(char key, bool down);
};

int isKeyDown(int id);
