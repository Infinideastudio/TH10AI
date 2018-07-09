#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include "Windows.hpp"
#include <tlhelp32.h>
#include <memory>
#include <ctime>
#include <thread>
#include <chrono>
#include "Vec2.hpp"
#include "Object.hpp"
#include "KeyboardManager.hpp"
#include "GameManager.hpp"

using namespace std;
using namespace std::chrono;

class StopWatch {
public:
    StopWatch() = default;
    ~StopWatch() = default;
public:
    void start() noexcept { mBeginTime = steady_clock::now(); }
    void stop() noexcept { mEndTime = steady_clock::now(); }
    void restart() noexcept { start(); }
    steady_clock::duration elapsed() const noexcept { return mEndTime - mBeginTime; }
    int elapsed_ms() const noexcept { return static_cast<int>(duration_cast<microseconds>(elapsed()).count()); };
    int elapsed_s() const noexcept { return static_cast<int>(duration_cast<seconds>(elapsed()).count()); };
private:
    steady_clock::time_point mBeginTime, mEndTime;
};

bool GetProcessIdByName(const char *exeFileName, int &pid) {
    pid = 0;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    bool res = Process32First(hsnap, &pe);
    //cout << res << endl;
    do {
        //printf("%s\n", pe.szExeFile);
        if (!strcmp(exeFileName, pe.szExeFile)) {
            pid = pe.th32ProcessID;
            break;
        }
    } while (Process32Next(hsnap, &pe));
    if (!pid) {

        return false;
    }
    return true;
}
void PointRotate(float cx, float cy, float &x, float &y, float arc) {
    float _x, _y;
    _x = cx + (x - cx) * cos(arc) - (y - cy) * sin(arc);
    _y = cy + (x - cx) * sin(arc) + (y - cy) * cos(arc);
    x = _x;
    y = _y;
}
void pauseUntilPress(const std::string &info, char key) {
    std::cout << info << std::endl;
    while (true) {
        if (isKeyDown(key))return;
        std::this_thread::sleep_for(10ms);
    }
}

int main(int argc, char **argv) {
    srand(static_cast<unsigned int>(time(nullptr)));
    KeyboardManager::init();
    int pid = 1;
    if (!GetProcessIdByName("th10.exe", pid)) {
        printf("th10.exe not running！\n");
        system("pause");
        return 0;
    }
    auto process = OpenProcess(PROCESS_VM_READ, true, pid);
    if (!process) {
        printf("cannot open th10 process！\n");
        system("pause");
        return 0;
    }
    auto game = std::make_shared<GameManager>(process);
    bool quit = false;
    std::cout << "准备完成" << std::endl;
    pauseUntilPress("请将焦点放在风神录窗口上，开始游戏，然后按C开启AI", 'C');
    std::cout << "已开始游戏，按Q键退出" << std::endl;
    unsigned long long frameCount = 0;
    while (!quit) {
        StopWatch watch;
        watch.start();
        frameCount++;
        game->update(frameCount);
        watch.stop();
        if (isKeyDown('Q'))
            quit = true;
        std::this_thread::sleep_for(microseconds(std::max(0, 14 - watch.elapsed_ms())));
    }
    KeyboardManager::sendKeyInfo(0, false, false, false);
    system("pause");
}
