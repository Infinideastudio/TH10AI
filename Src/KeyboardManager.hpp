#pragma once

struct KeyboardManager {
    static void init();
    static void sendKeyInfo(int dir, bool shift, bool z, bool x);
	static void pressEsc();
	static void releaseEsc();
};

int isKeyDown(int id);

