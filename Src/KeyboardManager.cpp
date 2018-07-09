#include "KeyboardManager.hpp"
#include <Windows.h>

bool KeyboardManager::keystate[100];

void KeyboardManager::sendKey(char key, bool down) {
    if (keystate[key] != down)
    {
        keystate[key] = down;
        if (down) keybd_event(key, MapVirtualKey(key, 0), 0, 0);
        else keybd_event(key, MapVirtualKey(key, 0), KEYEVENTF_KEYUP, 0);
    }
}

void KeyboardManager::sendKeyInfo(int dir, bool shift, bool z, bool x) {
    //std::cout << (bool)(dir & 0x8) << " " << (bool)(dir & 0x4) << " " << (bool)(dir & 0x2) << " " << (bool)(dir & 0x1) << std::endl;
    sendKey(KEY_UP, (bool)(dir & 0x8));
    sendKey(KEY_DOWN, (bool)(dir & 0x4));
    sendKey(KEY_LEFT, (bool)(dir & 0x2));
    sendKey(KEY_RIGHT, (bool)(dir & 0x1));
    sendKey(KEY_SHIFT, shift);
    sendKey(KEY_Z, z);
    sendKey(KEY_X, x);
}

void KeyboardManager::init() {
    memset(keystate, 0, sizeof(keystate));
}

int isKeyDown(int id) { return GetAsyncKeyState(id) & 0x8000 ? 1 : 0; }
