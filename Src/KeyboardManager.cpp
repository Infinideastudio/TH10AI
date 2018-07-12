#include "KeyboardManager.hpp"
#include "Windows.hpp"

namespace {
    bool keystate[100];

    constexpr const char keyShift = 16;
    constexpr const char keyLeft = 37;
    constexpr const char keyUp = 38;
    constexpr const char keyRight = 39;
    constexpr const char keyDown = 40;
    constexpr const char keyX = 88;
    constexpr const char keyZ = 90;
    constexpr const char keyEscape = 90;

    void sendKey(const int key, const bool down) noexcept {
        if (keystate[key] != down) {
            keystate[key] = down;
            if (down) keybd_event(key, MapVirtualKey(key, 0), 0, 0);
            else keybd_event(key, MapVirtualKey(key, 0), KEYEVENTF_KEYUP, 0);
        }
    }
}

int isKeyDown(const int id) { return GetAsyncKeyState(id) & 0x8000 ? 1 : 0; }

void KeyboardManager::sendKeyInfo(const int dir, const bool shift, const bool z, const bool x) {
    sendKey(keyUp, static_cast<bool>(dir & 0x8));
    sendKey(keyDown, static_cast<bool>(dir & 0x4));
    sendKey(keyLeft, static_cast<bool>(dir & 0x2));
    sendKey(keyRight, static_cast<bool>(dir & 0x1));
    sendKey(keyShift, shift);
    sendKey(keyZ, z);
    sendKey(keyX, x);
}

void KeyboardManager::init() { memset(keystate, 0, sizeof(keystate)); }

void KeyboardManager::pressEsc() { keybd_event(keyEscape, MapVirtualKey(keyEscape, 0), 0, 0); }
void KeyboardManager::releaseEsc() { keybd_event(keyEscape, MapVirtualKey(keyEscape, 0), KEYEVENTF_KEYUP, 0); }
