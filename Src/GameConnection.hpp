#pragma once

#include <memory>
#include <vector>
#include "Object.hpp"
#include "Windows.hpp"
enum class PlayerState:int
{
	NOTCREATED=0,
	NORMAL=1,
	INVINCIBLE =2,
	DYING=4
};
struct GameConnection {
    virtual ~GameConnection() noexcept = default;
	virtual int getTimeline() noexcept = 0;
    virtual void getPowers(std::vector<Object> &powers) noexcept = 0;
    virtual void getEnemyData(std::vector<Object> &enemy) noexcept = 0;
    virtual void getEnemyBulletData(std::vector<Object> &bullet, const Player &player, double maxRange) noexcept = 0;
    virtual void getPlayerData(Player &self) noexcept = 0;
    virtual void getEnemyLaserData(std::vector<Laser> &laser) noexcept = 0;
    virtual void sendKeyInfo(int dir,bool shift,bool z,bool x) noexcept = 0;
	virtual void getMousePosition(Vec2d& pos)noexcept = 0;
	virtual PlayerState GetPlayerStateInformation()noexcept = 0;
};
class GameConnectionTH10 : public GameConnection {
public:
	GameConnectionTH10();
	~GameConnectionTH10() noexcept override;
	void getPowers(std::vector<Object> &powers) noexcept override;
	void getEnemyData(std::vector<Object> &enemy) noexcept override;
	void getEnemyBulletData(std::vector<Object> &bullet, const Player &player, double maxRange) noexcept override;
	void getPlayerData(Player &self) noexcept override;
	void getEnemyLaserData(std::vector<Laser> &laser) noexcept override;
	void sendKeyInfo(int dir, bool shift, bool z, bool x) noexcept override;
	void getMousePosition(Vec2d& pos)noexcept override;
	int getTimeline() noexcept override;
	PlayerState GetPlayerStateInformation()noexcept override;
private:
	HWND mWindow{ nullptr };
	HANDLE mHProcess{ nullptr };
	static bool GetProcessIdByName(const char *exeFileName, DWORD &pid) noexcept;
	void readProcessRaw(intptr_t offset, size_t length, void *target) const noexcept {
		static thread_local SIZE_T nbr;
		ReadProcessMemory(mHProcess, reinterpret_cast<LPCVOID>(offset), target, length, &nbr);
	}
	template<class T>
	T readProcess(intptr_t offset) const noexcept {
		T result;
		readProcessRaw(offset, sizeof(T), &result);
		return result;
	}
};
std::unique_ptr<GameConnection> createGameConnection();
