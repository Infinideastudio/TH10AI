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

class stop_watch {
public:
    stop_watch()
            : elapsed_(0) {
        QueryPerformanceFrequency(&freq_);
    }
    ~stop_watch() = default;
public:
    void start() {
        QueryPerformanceCounter(&begin_time_);
    }
    void stop() {
        LARGE_INTEGER end_time;
        QueryPerformanceCounter(&end_time);
        elapsed_ += (end_time.QuadPart - begin_time_.QuadPart) * 1000000 / freq_.QuadPart;
    }
    void restart() {
        elapsed_ = 0;
        start();
    }
    //微秒
    double elapsed() {
        return static_cast<double>(elapsed_);
    }
    //毫秒
    double elapsed_ms() {
        return elapsed_ / 1000.0;
    }
    //秒
    double elapsed_second() {
        return elapsed_ / 1000000.0;
    }
private:
    LARGE_INTEGER freq_{};
    LARGE_INTEGER begin_time_{};
    long long elapsed_;
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
        stop_watch watch;
        watch.start();
        frameCount++;
        game->update(frameCount);
        watch.stop();
        if (isKeyDown('Q'))
            quit = true;
        std::this_thread::sleep_for(microseconds(std::max(0, 14 - static_cast<int>(watch.elapsed_ms()))));
    }
    KeyboardManager::sendKeyInfo(0, false, false, false);
    system("pause");
}
