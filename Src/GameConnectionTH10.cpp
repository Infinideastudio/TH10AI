#include "GameConnection.hpp"
#include "KeyboardManager.hpp"
#include <Windows.h>
#include <TlHelp32.h>

class GameConnectionTH10 : public GameConnection {
public:
    GameConnectionTH10();
    ~GameConnectionTH10() noexcept override;
    void GetPowers(std::vector<Object> &powers) noexcept override;
    void GetEnemyData(std::vector<Object> &enemy) noexcept override;
    void GetEnemyBulletData(std::vector<Object> &bullet, const Player &player, double maxRange) noexcept override;
    void GetPlayerData(Player &self) noexcept override;
    void GetEnemyLaserData(std::vector<Laser> &laser) noexcept override;
    void sendKeyInfo(int dir, bool shift, bool z, bool x) noexcept override;
private:
    HANDLE mHProcess{nullptr};
    static bool GetProcessIdByName(const char *exeFileName, DWORD &pid) noexcept;
    static double getSquareDis(Vec2d point1, Vec2d point2) {
        return (point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y);
    }
    void readProcessRaw(intptr_t offset, size_t length, void *target) const noexcept {
        static thread_local DWORD nbr;
        ReadProcessMemory(mHProcess, reinterpret_cast<LPCVOID>(offset), target, length, &nbr);
    }
    template<class T>
    T readProcess(intptr_t offset) const noexcept {
        T result;
        readProcessRaw(offset, sizeof(T), &result);
        return result;
    }
};

namespace {
    char staticBuffer[0x7F0 * 2001];
    template<class T>
    T read(void *buffer) noexcept { return *reinterpret_cast<T *>(buffer); }
}

void GameConnectionTH10::GetPowers(std::vector<Object> &powers) noexcept {
    powers.clear();
    auto ebp = staticBuffer;
    const auto base = readProcess<int32_t>(0x00477818);
    if (!base) return;
    readProcessRaw(base + 0x3C0, 0x3F0 * 2000, staticBuffer);
    for (auto i = 0; i < 2000; i++) {
        const auto eax = read<int32_t>(ebp + 0x30);
        if (eax == 1) {
            const auto x = read<float>(ebp), y = read<float>(ebp + 0x4);
            powers.emplace_back(x, y, 6, 6);
        }
        ebp += 0x3F0;
    }
}

void GameConnectionTH10::GetEnemyData(std::vector<Object> &enemy) noexcept {
    enemy.clear();
    const auto base = readProcess<int32_t>(0x00477704);
    if (!base) return;
    auto obj_base = readProcess<int32_t>(base + 0x58);
    if (obj_base) {
        while (true) {
            const auto obj_address = readProcess<int32_t>(obj_base) + 0x103C;
            const auto obj_next = readProcess<int32_t>(obj_base + 4);
            const auto t = readProcess<uint32_t>(obj_address + 0x1444);
            if (!(t & 0x40u) && !(t & 0x12u)) {
                const auto x = readProcess<float>(obj_address + 0x2C), y = readProcess<float>(obj_address + 0x30),
                        w = readProcess<float>(obj_address + 0xB8), h = readProcess<float>(obj_address + 0xBC);
                enemy.emplace_back(x, y, w, h);
            }
            if (!obj_next) break;
            obj_base = obj_next;
        }
    }
}

void
GameConnectionTH10::GetEnemyBulletData(std::vector<Object> &bullet, const Player &player, double maxRange) noexcept {
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
                    w = read<float>(ebx + 0x3F0), h = read<float>(ebx + 0x3F4),
                    dx = read<float>(ebx + 0x3C0), dy = read<float>(ebx + 0x3C4);
            //为了效率，只考虑可能会碰到的子弹
            if (getSquareDis(Vec2d(x, y), player.pos) <= maxRange * maxRange)
                bullet.emplace_back(x, y, w, h, dx / 2.0f, dy / 2.0f);
        }
        ebx += 0x7F0;
    }
}

void GameConnectionTH10::GetPlayerData(Player &self) noexcept {
    const auto obj_base = readProcess<int32_t>(0x00477834);
    if (!obj_base) return;
    self.pos.x = readProcess<float>(obj_base + 0x3C0);
    self.pos.y = readProcess<float>(obj_base + 0x3C4);
}

void GameConnectionTH10::GetEnemyLaserData(std::vector<Laser> &laser) noexcept {
    laser.clear();
    const auto base = readProcess<int32_t>(0x0047781C);
    if (!base) return;
    auto esi = readProcess<int32_t>(base + 0x18);
    if (esi) {
        while (true) {
            const auto ebx = readProcess<int32_t>(esi + 0x8);
            const auto x = readProcess<float>(esi + 0x24), y = readProcess<float>(esi + 0x28),
                    w = readProcess<float>(esi + 0x44), h = readProcess<float>(esi + 0x40),
                    arc = readProcess<float>(esi + 0x3C);
            laser.emplace_back(x, y, w / 2.0f, h, arc);
            if (!ebx) break;
            esi = ebx;
        }
    }
}

void GameConnectionTH10::sendKeyInfo(int32_t dir, bool shift, bool z, bool x) noexcept {
    KeyboardManager::sendKeyInfo(dir, shift, z, x);
}

GameConnectionTH10::~GameConnectionTH10() noexcept { CloseHandle(mHProcess); }

GameConnectionTH10::GameConnectionTH10() {
    DWORD pid = 1;
    if (!GetProcessIdByName("th10.exe", pid))
        throw std::runtime_error("th10.exe not running!");
    mHProcess = OpenProcess(PROCESS_VM_READ, true, pid);
    if (!mHProcess)
        throw std::runtime_error("cannot open th10 process!");
}

bool GameConnectionTH10::GetProcessIdByName(const char *exeFileName, DWORD &pid) noexcept {
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
    } while (Process32Next(hSnapshot, &pe));
    return static_cast<bool>(pid);
}

std::unique_ptr<GameConnection> createGameConnection() {
    return std::make_unique<GameConnectionTH10>();
}
