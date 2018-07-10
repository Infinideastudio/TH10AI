#include <thread>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "KeyboardManager.hpp"
#include "GameManager.hpp"

using namespace std;
using namespace std::chrono;

class StopWatch {
public:
    void start() noexcept { mBeginTime = steady_clock::now(); }
    void stop() noexcept { mEndTime = steady_clock::now(); }
    void restart() noexcept { start(); }
    steady_clock::duration elapsed() const noexcept { return mEndTime - mBeginTime; }
    int elapsed_ms() const noexcept { return static_cast<int>(duration_cast<milliseconds>(elapsed()).count()); }
    int elapsed_s() const noexcept { return static_cast<int>(duration_cast<seconds>(elapsed()).count()); }
private:
    steady_clock::time_point mBeginTime, mEndTime;
};


void pauseUntilPress(const char* info, char key) {
    std::cout << info << std::endl;
    while (true) {
        if (isKeyDown(key))return;
        std::this_thread::sleep_for(10ms);
    }
}

int main() {
    try {
        KeyboardManager::init();
        auto game = std::make_shared<GameManager>();
        bool quit = false;
        std::cout << "准备完成" << std::endl;
        pauseUntilPress("请将焦点放在风神录窗口上，开始游戏，然后按C开启AI", 'C');
        std::cout << "已开始游戏，按P键打印估价图，Q键退出" << std::endl;
        unsigned long long frameCount = 0;
        StopWatch watch;
        int mapOutputCount = 0;
        double cd = 0;
        const double maxcd = 10;
        while (!quit) {
            if (cd > 0)cd -= 1.0;
            if (isKeyDown('Q'))
                break;
            if (isKeyDown('P') && cd <= 0) {
                cd = maxcd;
                mapOutputCount++;
                std::stringstream outputStream;
                outputStream << "./value" << mapOutputCount << ".bmp";
                std::cout << outputStream.str() << std::endl;
                game->outputValueMap(outputStream.str().c_str());
                continue;
            }
            watch.start();
            frameCount++;
            game->update(frameCount);
            watch.stop();
            std::this_thread::sleep_for(milliseconds(std::max(0, 16 - watch.elapsed_ms())));
        }
        KeyboardManager::sendKeyInfo(0, false, false, false);
    }
    catch (std::exception& e) { std::cout << e.what() << std::endl; }
    system("pause");
}
