#include <thread>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include "GameConnection.hpp"
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
    int elapsedMs() const noexcept { return static_cast<int>(duration_cast<milliseconds>(elapsed()).count()); }
    int elapsedS() const noexcept { return static_cast<int>(duration_cast<seconds>(elapsed()).count()); }
private:
    steady_clock::time_point mBeginTime, mEndTime;
};


void pauseUntilPress(const char* info, const char key) {
    std::cout << info << std::endl;
    while (true) {
        if (isKeyDown(key))return;
        std::this_thread::sleep_for(10ms);
    }
}
GameConnectionTH10 frameSyncer;
int main() {
    try {
        KeyboardManager::init();
        auto game = std::make_shared<GameManager>();
        const auto quit = false;
        std::cout << "准备完成" << std::endl;
        pauseUntilPress("请将焦点放在风神录窗口上，开始游戏，按C开启AI", 'C');
        std::cout << "已开始游戏，按C键开启/关闭鼠标引导，Q键退出" << std::endl;
        unsigned long long frameCount = 0;
        StopWatch watch;
<<<<<<< HEAD
        int mapOutputCount = 0;
        const double maxcd = 30;
		double cd = 30;
		bool mouseMode = false;
		cout << "鼠标引导当前处于关闭状态" << endl;
		int gameFrame = frameSyncer.getTimeline();
		unsigned long long loopCount = 0;
        while (!quit) {
			if (loopCount % 16 == 0)
			{		
				if (cd > 0)cd -= 1.0;
				if (isKeyDown('Q'))
					break;
				if (cd <= 0 && isKeyDown('C'))
				{
					cd = maxcd;
					mouseMode ^= 1;
					cout << (mouseMode ? "鼠标引导已开启" : "鼠标引导已关闭") << endl;
				}
				if (isKeyDown('P') && cd <= 0) {
					cd = maxcd;
					mapOutputCount++;
					std::stringstream outputStream;
					outputStream << "./value" << mapOutputCount << ".bmp";
					std::cout << outputStream.str() << std::endl;
					game->outputValueMap(outputStream.str().c_str());
					continue;
				}
			}
			int getGameFrame = frameSyncer.getTimeline();
			if (getGameFrame != gameFrame)
			{
				if (getGameFrame > gameFrame + 1)
				{
					cout << "Frame Lost!" << endl;
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
=======
        auto mapOutputCount = 0;
        auto cd = 0.0;
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
            std::this_thread::sleep_for(milliseconds(std::max(0, 16 - watch.elapsedMs())));
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
        }
        KeyboardManager::sendKeyInfo(0, false, false, false);
    }
    catch (std::exception& e) { std::cout << e.what() << std::endl; }
    system("pause");
}
