#include "KeyboardManager.hpp"
#include "Windows.hpp"

namespace {
    bool keystate[100];

    constexpr const char KEY_SHIFT = 16;
    constexpr const char KEY_LEFT = 37;
    constexpr const char KEY_UP = 38;
    constexpr const char KEY_RIGHT = 39;
    constexpr const char KEY_DOWN = 40;
    constexpr const char KEY_X = 88;
    constexpr const char KEY_Z = 90;
	constexpr const char KEY_ESCAPE = 90;
    void sendKey(int key, bool down) noexcept {	
        if (keystate[key] != down) {
            keystate[key] = down;
            if (down) keybd_event(key, MapVirtualKey(key, 0), 0, 0);
            else keybd_event(key, MapVirtualKey(key, 0), KEYEVENTF_KEYUP, 0);
        }
    }
}

int isKeyDown(int id) { return GetAsyncKeyState(id) & 0x8000 ? 1 : 0; }

void KeyboardManager::sendKeyInfo(int dir, bool shift, bool z, bool x) {
    sendKey(KEY_UP, static_cast<bool>(dir & 0x8));
    sendKey(KEY_DOWN, static_cast<bool>(dir & 0x4));
    sendKey(KEY_LEFT, static_cast<bool>(dir & 0x2));
    sendKey(KEY_RIGHT, static_cast<bool>(dir & 0x1));
    sendKey(KEY_SHIFT, shift);
    sendKey(KEY_Z, z);
    sendKey(KEY_X, x);
}

void KeyboardManager::init() { memset(keystate, 0, sizeof(keystate)); }

void KeyboardManager::pressEsc() {
	keybd_event(KEY_ESCAPE, MapVirtualKey(KEY_ESCAPE, 0), 0, 0);
}
void KeyboardManager::releaseEsc() {
	keybd_event(KEY_ESCAPE, MapVirtualKey(KEY_ESCAPE, 0), KEYEVENTF_KEYUP, 0);
}