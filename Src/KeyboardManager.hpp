#pragma once

struct KeyboardManager {
    static void init();
    static void sendKeyInfo(int dir, bool shift, bool z, bool x);
};

int isKeyDown(int id);
