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
};

namespace { char staticBuffer[0x7F0 * 2001]; }

void GameConnectionTH10::GetPowers(std::vector<Object> &powers) noexcept {
    DWORD nbr;
    powers.clear();
    int base;
    char* ebp = staticBuffer;
    ReadProcessMemory(mHProcess, (LPCVOID) 0x00477818, &base, 4, &nbr);
    if (base == NULL) {
        return;
    }
    ReadProcessMemory(mHProcess, (LPCVOID) (base + 0x3C0), staticBuffer, 0x3F0 * 2000, &nbr);
    for (int i = 0; i < 2000; i++) {
        const int eax = (*reinterpret_cast<int*>(ebp + 0x30));
        if (eax==1) {
            const float x = *reinterpret_cast<float*>(ebp), y = *reinterpret_cast<float*>(ebp + 0x4);
            powers.emplace_back(x, y, 6, 6);
        }
        ebp += 0x3F0;
    }
}

void GameConnectionTH10::GetEnemyData(std::vector<Object> &enemy) noexcept {
    DWORD nbr;
    int base, obj_base, obj_addr, obj_next;
    enemy.clear();
    ReadProcessMemory(mHProcess, (LPCVOID) 0x00477704, &base, 4, &nbr);
    if (base == NULL) {
        return;
    }
    ReadProcessMemory(mHProcess, (LPCVOID) (base + 0x58), &obj_base, 4, &nbr);
    if (obj_base != NULL) {
        while (true) {
            ReadProcessMemory(mHProcess, (LPCVOID) obj_base, &obj_addr, 4, &nbr);
            ReadProcessMemory(mHProcess, (LPCVOID) (obj_base + 4), &obj_next, 4, &nbr);
            obj_addr += 0x103C;
            unsigned int t;
            ReadProcessMemory(mHProcess, (LPCVOID) (obj_addr + 0x1444), &t, 4, &nbr);
            if (!(t & 0x40)) {
                ReadProcessMemory(mHProcess, (LPCVOID) (obj_addr + 0x1444), &t, 4, &nbr);
                if (!(t & 0x12)) {
                    float x, y, w, h;
                    ReadProcessMemory(mHProcess, (LPCVOID) (obj_addr + 0x2C), &x, 4, &nbr);
                    ReadProcessMemory(mHProcess, (LPCVOID) (obj_addr + 0x30), &y, 4, &nbr);
                    ReadProcessMemory(mHProcess, (LPCVOID) (obj_addr + 0xB8), &w, 4, &nbr);
                    ReadProcessMemory(mHProcess, (LPCVOID) (obj_addr + 0xBC), &h, 4, &nbr);
                    enemy.emplace_back(x, y, w, h);
                }
            }
            if (obj_next == 0) {
                break;
            }
            obj_base = obj_next;
        }
    }
}

void
GameConnectionTH10::GetEnemyBulletData(std::vector<Object> &bullet, const Player& player, double maxRange) noexcept {
    bullet.clear();
    int base, eax;
    DWORD nbr;
    char* ebx = staticBuffer;
    ReadProcessMemory(mHProcess, (LPCVOID) 0x004776F0, &base, 4, &nbr);
    if (!base) return;
    ReadProcessMemory(mHProcess, (LPCVOID) 0x00477810, &eax, 4, &nbr);
    if (eax) {
        ReadProcessMemory(mHProcess, (LPCVOID) (eax + 0x58), &eax, 4, &nbr);
        if (eax & 0x00000400) return;
    }
    ReadProcessMemory(mHProcess, (LPCVOID) (base + 0x60), staticBuffer, 0x7F0 * 2000, &nbr);
    for (int i = 0; i < 2000; i++) {
        const int bp = (*reinterpret_cast<int*>(ebx + 0x446))  & 0x0000FFFF;
        if (bp) {
            const float x = *reinterpret_cast<float*>(ebx + 0x3B4), y = *reinterpret_cast<float*>(ebx + 0x3B8),
                    w = *reinterpret_cast<float*>(ebx + 0x3F0), h = *reinterpret_cast<float*>(ebx + 0x3F4),
                    dx = *reinterpret_cast<float*>(ebx + 0x3C0), dy = *reinterpret_cast<float*>(ebx + 0x3C4);
            //为了效率，只考虑可能会碰到的子弹
            if (getSquareDis(Vec2d(x, y), player.pos) <= maxRange * maxRange)
                bullet.emplace_back(x, y, w, h, dx / 2.0f, dy / 2.0f);
        }
        ebx += 0x7F0;
    }
}

void GameConnectionTH10::GetPlayerData(Player &self) noexcept {
    float x, y;
    int obj_base;
    DWORD nbr;
    ReadProcessMemory(mHProcess, (LPCVOID) 0x00477834, &obj_base, 4, &nbr);
    if (obj_base == NULL) {
        return;
    }
    ReadProcessMemory(mHProcess, (LPCVOID) (obj_base + 0x3C0), &x, 4, &nbr);
    ReadProcessMemory(mHProcess, (LPCVOID) (obj_base + 0x3C4), &y, 4, &nbr);
    self.pos.x = x;
    self.pos.y = y;
}

void GameConnectionTH10::GetEnemyLaserData(std::vector<Laser> &laser) noexcept {
    laser.clear();
    int base;
    DWORD nbr;
    ReadProcessMemory(mHProcess, (LPCVOID) 0x0047781C, &base, 4, &nbr);
    if (base == NULL) {
        return;
    }
    int esi, ebx;
    ReadProcessMemory(mHProcess, (LPCVOID) (base + 0x18), &esi, 4, &nbr);
    if (esi != NULL) {
        while (true) {
            ReadProcessMemory(mHProcess, (LPCVOID) (esi + 0x8), &ebx, 4, &nbr);
            float x, y, h, w, arc;
            ReadProcessMemory(mHProcess, (LPCVOID) (esi + 0x24), &x, 4, &nbr);
            ReadProcessMemory(mHProcess, (LPCVOID) (esi + 0x28), &y, 4, &nbr);
            ReadProcessMemory(mHProcess, (LPCVOID) (esi + 0x3C), &arc, 4, &nbr);
            ReadProcessMemory(mHProcess, (LPCVOID) (esi + 0x40), &h, 4, &nbr);
            ReadProcessMemory(mHProcess, (LPCVOID) (esi + 0x44), &w, 4, &nbr);
            laser.emplace_back(x, y, w / 2.0f, h, arc);
            if (ebx == NULL) {
                break;
            }
            esi = ebx;
        }
    }
}

void GameConnectionTH10::sendKeyInfo(int dir, bool shift, bool z, bool x) noexcept {
    KeyboardManager::sendKeyInfo(dir, shift, z, x);
}

GameConnectionTH10::~GameConnectionTH10() noexcept { CloseHandle(mHProcess); }

GameConnectionTH10::GameConnectionTH10() {
    DWORD pid = 1;
    if (!GetProcessIdByName("th10.exe", pid))
        throw std::runtime_error("th10.exe not running!");
    mHProcess = OpenProcess(PROCESS_VM_READ, true, pid);
    if (!mHProcess)
        std::runtime_error("cannot open th10 process!");
}

bool GameConnectionTH10::GetProcessIdByName(const char *exeFileName, DWORD &pid) noexcept {
    pid = 0;
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    auto res = Process32First(hsnap, &pe);
    do {
        if (!strcmp(exeFileName, pe.szExeFile)) {
            pid = pe.th32ProcessID;
            break;
        }
    } while (Process32Next(hsnap, &pe));
    return static_cast<bool>(pid);
}

std::unique_ptr<GameConnection> createGameConnection() {
    return std::make_unique<GameConnectionTH10>();
}
