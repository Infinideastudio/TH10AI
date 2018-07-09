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
	static void init()
	{
		memset(keystate, 0, sizeof(keystate));
	}
	static void sendKeyInfo(int dir,bool shift,bool z,bool x)
	{
		//std::cout << (bool)(dir & 0x8) << " " << (bool)(dir & 0x4) << " " << (bool)(dir & 0x2) << " " << (bool)(dir & 0x1) << std::endl;
		sendKey(KEY_UP, (bool)(dir&0x8));
		sendKey(KEY_DOWN, (bool)(dir & 0x4));
		sendKey(KEY_LEFT, (bool)(dir & 0x2));
		sendKey(KEY_RIGHT, (bool)(dir & 0x1));
		sendKey(KEY_SHIFT, shift);
		sendKey(KEY_Z, z);
		sendKey(KEY_X, x);
	}
private:
	static bool keystate[100];
	static void sendKey(char key, bool down)
	{
		if (keystate[key] != down)
		{
			keystate[key] = down;
			if (down) keybd_event(key, MapVirtualKey(key, 0), 0, 0);
			else keybd_event(key, MapVirtualKey(key, 0), KEYEVENTF_KEYUP, 0);
		}
	}
};
bool KeyboardManager::keystate[100];