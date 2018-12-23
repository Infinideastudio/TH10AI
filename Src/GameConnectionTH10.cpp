#include "GameConnection.hpp"
#include "KeyboardManager.hpp"

#include <TlHelp32.h>

<<<<<<< HEAD
=======
class GameConnectionTh10 : public GameConnection {
public:
    GameConnectionTh10();
    ~GameConnectionTh10() noexcept override;
    void getPowers(std::vector<Object>& powers) noexcept override;
    void getEnemyData(std::vector<Object>& enemy) noexcept override;
    void getEnemyBulletData(std::vector<Object>& bullet, const Player& player, double maxRange) noexcept override;
    void getPlayerData(Player& self) noexcept override;
    void getEnemyLaserData(std::vector<Laser>& laser) noexcept override;
    void sendKeyInfo(int dir, bool shift, bool z, bool x) noexcept override;
private:
    HANDLE mHProcess{nullptr};
    static bool getProcessIdByName(const char* exeFileName, DWORD& pid) noexcept;

    void readProcessRaw(const intptr_t offset, const size_t length, void* target) const noexcept {
        static thread_local SIZE_T nbr;
        ReadProcessMemory(mHProcess, reinterpret_cast<LPCVOID>(offset), target, length, &nbr);
    }

    template <class T>
    T readProcess(const intptr_t offset) const noexcept {
        T result;
        readProcessRaw(offset, sizeof(T), &result);
        return result;
    }
};
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56

namespace {
    char staticBuffer[0x7F0 * 2001];

    template <class T>
    T read(void* buffer) noexcept { return *reinterpret_cast<T *>(buffer); }
}

<<<<<<< HEAD
void GameConnectionTH10::getPowers(std::vector<Object> &powers) noexcept {
=======
void GameConnectionTh10::getPowers(std::vector<Object>& powers) noexcept {
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    powers.clear();
    auto ebp = staticBuffer;
    const auto base = readProcess<int32_t>(0x00477818);
    if (!base) return;
    readProcessRaw(base + 0x3C0, 0x3F0 * 2000, staticBuffer);
    for (auto i = 0; i < 2000; i++) {
        const auto eax = read<int32_t>(ebp + 0x30);
        if (eax == 1) {
            const auto x = read<float>(ebp), y = read<float>(ebp + 0x4);
            powers.emplace_back(Vec2d{x, y}, Vec2d{6, 6});
        }
        ebp += 0x3F0;
    }
}

<<<<<<< HEAD
void GameConnectionTH10::getEnemyData(std::vector<Object> &enemy) noexcept {
=======
void GameConnectionTh10::getEnemyData(std::vector<Object>& enemy) noexcept {
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    enemy.clear();
    const auto base = readProcess<int32_t>(0x00477704);
    if (!base) return;
    auto objBase = readProcess<int32_t>(base + 0x58);
    if (objBase) {
        while (true) {
            const auto objAddress = readProcess<int32_t>(objBase) + 0x103C;
            const auto objNext = readProcess<int32_t>(objBase + 4);
            const auto t = readProcess<uint32_t>(objAddress + 0x1444);
            if (!(t & 0x40u) && !(t & 0x12u)) {
                const auto x = readProcess<float>(objAddress + 0x2C), y = readProcess<float>(objAddress + 0x30),
                           w = readProcess<float>(objAddress + 0xB8), h = readProcess<float>(objAddress + 0xBC);
                enemy.emplace_back(Vec2d{x, y}, Vec2d{w, h});
            }
            if (!objNext) break;
            objBase = objNext;
        }
    }
}
<<<<<<< HEAD
void GameConnectionTH10::getMousePosition(Vec2d& pos)noexcept{
	const Vec2d orgdelta = Vec2d(28, 45);
	POINT m;
	GetCursorPos(&m);
	RECT rect;
	GetWindowRect(mWindow, &rect);
	Vec2d org = Vec2d(rect.left + orgdelta.x + 200.0, rect.top + orgdelta.y);
	pos = Vec2d(m.x - org.x, m.y - org.y);
}
void
GameConnectionTH10::getEnemyBulletData(std::vector<Object> &bullet, const Player &player, double maxRange) noexcept {
=======

void GameConnectionTh10::getEnemyBulletData(std::vector<Object>& bullet, const Player& player,
                                            const double maxRange) noexcept {
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    bullet.clear();
    auto ebx = staticBuffer;
    const auto base = readProcess<int32_t>(0x004776F0);
    if (!base) return;
    auto eax = readProcess<uint32_t>(0x00477810);
    if (eax) {
        eax = readProcess<uint32_t>(eax + 0x58);
        if (eax & 0x00000400u) return;
    }
    readProcessRaw(base + 0x60, 0x7F0 * 2000, staticBuffer);
    for (auto i = 0; i < 2000; i++) {
        const auto bp = read<uint32_t>(ebx + 0x446) & 0x0000FFFFu;
        if (bp) {
            const auto x = read<float>(ebx + 0x3B4), y = read<float>(ebx + 0x3B8),
<<<<<<< HEAD
                    w = read<float>(ebx + 0x3F0), h = read<float>(ebx + 0x3F4),
                    dx = read<float>(ebx + 0x3C0), dy = read<float>(ebx + 0x3C4);
            if (distanceSqr(Vec2d(x, y), player.pos) <= maxRange * maxRange)
                bullet.emplace_back(x, y, w, h, dx, dy);
=======
                       w = read<float>(ebx + 0x3F0), h = read<float>(ebx + 0x3F4),
                       dx = read<float>(ebx + 0x3C0), dy = read<float>(ebx + 0x3C4);
            //‰∏∫‰∫ÜÊïàÁéáÔºåÂè™ËÄÉËôëÂèØËÉΩ‰ºöÁ¢∞Âà∞ÁöÑÂ≠êÂºπ
            if (distanceSqr(Vec2d(x, y), player.pos) <= maxRange * maxRange)
                bullet.emplace_back(Vec2d{x, y}, Vec2d{w, h}, Vec2d{dx / 2.0f, dy / 2.0f});
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
        }
        ebx += 0x7F0;
    }
}

<<<<<<< HEAD
void GameConnectionTH10::getPlayerData(Player &self) noexcept {
    const auto obj_base = readProcess<int32_t>(0x00477834);
    if (!obj_base) return;
    self.pos.x = readProcess<float>(obj_base + 0x3C0);
    self.pos.y = readProcess<float>(obj_base + 0x3C4);
	self.size.x = 2.0;
	self.size.y = 2.0;
}

void GameConnectionTH10::getEnemyLaserData(std::vector<Laser> &laser) noexcept {
=======
void GameConnectionTh10::getPlayerData(Player& self) noexcept {
    const auto objBase = readProcess<int32_t>(0x00477834);
    if (!objBase) return;
    self.pos.x = readProcess<float>(objBase + 0x3C0);
    self.pos.y = readProcess<float>(objBase + 0x3C4);
}

void GameConnectionTh10::getEnemyLaserData(std::vector<Laser>& laser) noexcept {
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    laser.clear();
    const auto base = readProcess<int32_t>(0x0047781C);
    if (!base) return;
    auto esi = readProcess<int32_t>(base + 0x18);
    if (esi) {
        while (true) {
            const auto ebx = readProcess<int32_t>(esi + 0x8);
            const auto x = readProcess<float>(esi + 0x24), y = readProcess<float>(esi + 0x28),
<<<<<<< HEAD
                    w = readProcess<float>(esi + 0x44), h = readProcess<float>(esi + 0x40),
                    arc = readProcess<float>(esi + 0x3C);
            laser.emplace_back(x, y, w, h, arc);
=======
                       w = readProcess<float>(esi + 0x44), h = readProcess<float>(esi + 0x40),
                       arc = readProcess<float>(esi + 0x3C);
            laser.emplace_back(Vec2d{x, y}, Vec2d{w / 2.0f, h}, arc);
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
            if (!ebx) break;
            esi = ebx;
        }
    }
}
<<<<<<< HEAD
PlayerState GameConnectionTH10::GetPlayerStateInformation()noexcept {
	int base_addr = readProcess<int32_t>(0x00477834);
	if (!base_addr)
	{
		return (PlayerState)0;
	}
	return (PlayerState)readProcess<int32_t>(base_addr + 0x458);
}
int GameConnectionTH10::getTimeline() noexcept {
	return readProcess<int32_t>(0x00474C88);
}
void GameConnectionTH10::sendKeyInfo(int32_t dir, bool shift, bool z, bool x) noexcept {
=======

void GameConnectionTh10::sendKeyInfo(const int32_t dir, const bool shift, const bool z, const bool x) noexcept {
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    KeyboardManager::sendKeyInfo(dir, shift, z, x);
}

GameConnectionTh10::~GameConnectionTh10() noexcept {
    GameConnectionTh10::sendKeyInfo(0, false, false, false);
    CloseHandle(mHProcess);
}

GameConnectionTh10::GameConnectionTh10() {
    DWORD pid = 1;
    if (!getProcessIdByName("th10.exe", pid))
        throw std::runtime_error("th10.exe not running!");
    mHProcess = OpenProcess(PROCESS_VM_READ, true, pid);
    if (!mHProcess)
        throw std::runtime_error("cannot open th10 process!");
	mWindow = FindWindow(0, "ìåï˚ïóê_ò^Å@Å` Mountain of Faith. ver 1.00a");
	if (!mWindow)
		throw std::runtime_error("cannot open th10 window!");
}

bool GameConnectionTh10::getProcessIdByName(const char* exeFileName, DWORD& pid) noexcept {
    pid = 0;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    const auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    Process32First(hSnapshot, &pe);
    do {
        if (!strcmp(exeFileName, pe.szExeFile)) {
            pid = pe.th32ProcessID;
            break;
        }
    }
    while (Process32Next(hSnapshot, &pe));
    return static_cast<bool>(pid);
}

std::unique_ptr<GameConnection> createGameConnection() { return std::make_unique<GameConnectionTh10>(); }
