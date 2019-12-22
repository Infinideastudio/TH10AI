#include <thread>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "GameConnection.hpp"
#include "KeyboardManager.hpp"
#include "GameManager.hpp"

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
        pauseUntilPress("请将焦点放在风神录窗口上，开始游戏，按C开启AI", 'C');
        std::cout << "已开始游戏，按C键开启/关闭鼠标引导，Q键退出" << std::endl;
        unsigned long long frameCount = 0;
        StopWatch watch;
        int mapOutputCount = 0;
        const double maxCd = 30;
		double cd = 30;
		bool mouseMode = false;
		std::cout << "鼠标引导当前处于关闭状态" << std::endl;
		int gameFrame = game->getTimeline();
		unsigned long long loopCount = 0;
        while (!quit) {
			if (loopCount % 16 == 0){		
				if (cd > 0)cd -= 1.0;
				if (isKeyDown('Q'))
					break;
				if (cd <= 0 && isKeyDown('C')){
					cd = maxCd;
					mouseMode ^= 1;
					std::cout << (mouseMode ? "鼠标引导已开启" : "鼠标引导已关闭") << std::endl;
				}
				if (isKeyDown('P') && cd <= 0) {
					cd = maxCd;
					mapOutputCount++;
					std::stringstream outputStream;
					outputStream << "./value" << mapOutputCount << ".bmp";
					std::cout << outputStream.str() << std::endl;
					game->outputValueMap(outputStream.str().c_str());
					continue;
				}
			}
			int getGameFrame = game->getTimeline();
			if (getGameFrame != gameFrame){
				if (getGameFrame > gameFrame + 1){
					std::cout << "Frame Lost!" << std::endl;
				}
				gameFrame = getGameFrame;
				//watch.start();
				frameCount++;
				game->update(frameCount, mouseMode);
				//watch.stop();
				//cout << watch.elapsed_ms() / 16.0 * 100.0 << "%" << endl;
			}
			loopCount++;
            std::this_thread::sleep_for(milliseconds(1));
        }
        KeyboardManager::sendKeyInfo(0, false, false, false);
    }
    catch (std::exception& e) { std::cout << e.what() << std::endl; }
    system("pause");
}
