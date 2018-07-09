#include <cstdio>
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include <memory>
#include <ctime>
#include "Vec2.hpp"
#include "Object.hpp"
#include "KeyboardManager.hpp"
#define SDL_MAIN_HANDLED  
#include <SDL.h>
#include "GameManager.hpp"
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
using namespace std;
class stop_watch
{
public:
	stop_watch()
		: elapsed_(0)
	{
		QueryPerformanceFrequency(&freq_);
	}
	~stop_watch() {}
public:
	void start()
	{
		QueryPerformanceCounter(&begin_time_);
	}
	void stop()
	{
		LARGE_INTEGER end_time;
		QueryPerformanceCounter(&end_time);
		elapsed_ += (end_time.QuadPart - begin_time_.QuadPart) * 1000000 / freq_.QuadPart;
	}
	void restart()
	{
		elapsed_ = 0;
		start();
	}
	//微秒
	double elapsed()
	{
		return static_cast<double>(elapsed_);
	}
	//毫秒
	double elapsed_ms()
	{
		return elapsed_ / 1000.0;
	}
	//秒
	double elapsed_second()
	{
		return elapsed_ / 1000000.0;
	}
private:
	LARGE_INTEGER freq_;
	LARGE_INTEGER begin_time_;
	long long elapsed_;
};
bool GetProcessIdByName(const char * exeFileName, int& pid)
{
	pid = 0;
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(pe);
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	bool res=Process32First(hsnap, &pe);
	//cout << res << endl;
	do
	{
		//printf("%s\n", pe.szExeFile);
		if (!strcmp(exeFileName, pe.szExeFile))
		{
			pid = pe.th32ProcessID;
			break;
		}
	} while (Process32Next(hsnap, &pe));
	if (!pid)
	{
		
		return false;
	}
	return true;
}
void PointRotate(float cx, float cy, float& x, float& y, float arc)
{
	float _x, _y;
	_x = cx + (x - cx) * cos(arc) - (y - cy) * sin(arc);
	_y = cy + (x - cx) * sin(arc) + (y - cy) * cos(arc);
	x = _x;
	y = _y;
}
void pauseUntilPress(std::string info,char key)
{
	std::cout << info << std::endl;
	while(true)
	{ 
		if (KEY_DOWN(key))return;
		SDL_Delay(10);
	}
}
std::shared_ptr<GameManager> game;
std::shared_ptr<HANDLE> hprocess;
int main(int argc, char** argv)
{
	srand(time(0));
	KeyboardManager::init();
	int pid=1;
	if (!GetProcessIdByName("th10.exe", pid))
	{
		printf("th10.exe not running！\n");
		system("pause");
		return 0;
	}
	hprocess=std::make_shared<HANDLE>();
	*hprocess=OpenProcess(PROCESS_VM_READ, true, pid);
	if (!*hprocess)
	{
		printf("cannot open th10 process！\n");
		system("pause");
		return 0;
	}
	game= std::make_shared<GameManager>(hprocess);
	//SDL_Window* win = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 400, 480, SDL_WINDOW_SHOWN);
	//SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	//SDL_Event e;
	bool quit = false;
	std::cout << "准备完成" << std::endl;
	pauseUntilPress("请将焦点放在风神录窗口上，开始游戏，然后按C开启AI",'C');
	std::cout << "已开始游戏，按Q键退出" << std::endl;
	unsigned long long frameCount = 0;
	while (!quit)
	{
		stop_watch watch;
		watch.start();
		frameCount++;
		game->update(frameCount);
		watch.stop();
		if (KEY_DOWN('Q'))quit=true;
		//printf("%.2lf\n", watch.elapsed_ms());
		SDL_Delay(max(0,14-watch.elapsed_ms()));
	}
	KeyboardManager::sendKeyInfo(0, 0, 0, 0);
	//HWND hq = FindWindow(NULL, "QQ2012");
	//SetFocus(hq);
	system("pause");
	//SDL_DestroyRenderer(renderer);
	//SDL_DestroyWindow(win);
}
