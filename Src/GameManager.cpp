#include "GameManager.hpp"
#include "KeyboardManager.hpp"

void GameManager::GetEnemyData(std::vector<Object> &enemy) {
    DWORD nbr;
    int base, obj_base, obj_addr, obj_next;
    enemy.clear();
    ReadProcessMemory(mHprocess, (LPCVOID) 0x00477704, &base, 4, &nbr);
    if (base == NULL) {
        return;
    }
    ReadProcessMemory(mHprocess, (LPCVOID) (base + 0x58), &obj_base, 4, &nbr);
    if (obj_base != NULL) {
        while (true) {
            ReadProcessMemory(mHprocess, (LPCVOID) obj_base, &obj_addr, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID) (obj_base + 4), &obj_next, 4, &nbr);
            obj_addr += 0x103C;
            unsigned int t;
            ReadProcessMemory(mHprocess, (LPCVOID) (obj_addr + 0x1444), &t, 4, &nbr);
            if (!(t & 0x40)) {
                ReadProcessMemory(mHprocess, (LPCVOID) (obj_addr + 0x1444), &t, 4, &nbr);
                if (!(t & 0x12)) {
                    float x, y, w, h;
                    ReadProcessMemory(mHprocess, (LPCVOID) (obj_addr + 0x2C), &x, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID) (obj_addr + 0x30), &y, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID) (obj_addr + 0xB8), &w, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID) (obj_addr + 0xBC), &h, 4, &nbr);
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

void GameManager::GetEnemyBulletData(std::vector<Object> &bullet) {
    bullet.clear();
    int base;
    DWORD nbr;
    ReadProcessMemory(mHprocess, (LPCVOID) 0x004776F0, &base, 4, &nbr);
    if (base == NULL) {
        return;
    }
    int ebx = base + 0x60;
    for (int i = 0; i < 2000; i++) {
        int edi = ebx + 0x400;
        int bp;
        ReadProcessMemory(mHprocess, (LPCVOID) (edi + 0x46), &bp, 4, &nbr);
        bp = bp & 0x0000FFFF;
        if (bp) {
            int eax;
            ReadProcessMemory(mHprocess, (LPCVOID) (0x477810), &eax, 4, &nbr);
            if (eax != NULL) {
                ReadProcessMemory(mHprocess, (LPCVOID) (eax + 0x58), &eax, 4, &nbr);
                if (!(eax & 0x00000400)) {
                    float x, y, w, h, dx, dy;
                    ReadProcessMemory(mHprocess, (LPCVOID) (ebx + 0x3C0), &dx, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID) (ebx + 0x3C4), &dy, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID) (ebx + 0x3B4), &x, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID) (ebx + 0x3B8), &y, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID) (ebx + 0x3F0), &w, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID) (ebx + 0x3F4), &h, 4, &nbr);
                    bullet.emplace_back(x, y, w, h, dx / 2.0f, dy / 2.0f);
                }
            }
        }
        ebx += 0x7F0;
    }
}

void GameManager::GetPlayerData(Player &self) {
    float x, y;
    int obj_base;
    DWORD nbr;
    ReadProcessMemory(mHprocess, (LPCVOID) 0x00477834, &obj_base, 4, &nbr);
    if (obj_base == NULL) {
        return;
    }
    ReadProcessMemory(mHprocess, (LPCVOID) (obj_base + 0x3C0), &x, 4, &nbr);
    ReadProcessMemory(mHprocess, (LPCVOID) (obj_base + 0x3C4), &y, 4, &nbr);
    self.pos.x = x;
    self.pos.y = y;
}

void GameManager::GetPowers(std::vector<Object> &powers) {
    DWORD nbr;
    powers.clear();
    int base;
    ReadProcessMemory(mHprocess, (LPCVOID) 0x00477818, &base, 4, &nbr);
    if (base == NULL) {
        return;
    }
    int esi = base + 0x14;
    int ebp = esi + 0x3B0;

    for (int i = 0; i < 2000; i++) {
        int eax;
        ReadProcessMemory(mHprocess, (LPCVOID) (ebp + 0x2C), &eax, 4, &nbr);
        if (eax != 0) {
            float x, y;
            ReadProcessMemory(mHprocess, (LPCVOID) (ebp - 0x4), &x, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID) (ebp), &y, 4, &nbr);
            powers.emplace_back(x, y, 6, 6);
        }
        ebp += 0x3F0;
    }
}

void GameManager::GetEnemyLaserData(std::vector<Laser> &laser) {
    laser.clear();
    int base;
    DWORD nbr;
    ReadProcessMemory(mHprocess, (LPCVOID) 0x0047781C, &base, 4, &nbr);
    if (base == NULL) {
        return;
    }
    int esi, ebx;
    ReadProcessMemory(mHprocess, (LPCVOID) (base + 0x18), &esi, 4, &nbr);
    if (esi != NULL) {
        while (true) {
            ReadProcessMemory(mHprocess, (LPCVOID) (esi + 0x8), &ebx, 4, &nbr);
            float x, y, h, w, arc;
            ReadProcessMemory(mHprocess, (LPCVOID) (esi + 0x24), &x, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID) (esi + 0x28), &y, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID) (esi + 0x3C), &arc, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID) (esi + 0x40), &h, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID) (esi + 0x44), &w, 4, &nbr);
            laser.emplace_back(x, y, w / 2.0f, h, arc);
            if (ebx == NULL) {
                break;
            }
            esi = ebx;
        }
    }
}

void GameManager::update(unsigned long long frameCount) {
    this->GetEnemyData(mEnemy);
    this->GetEnemyBulletData(mBullet);
    this->GetEnemyLaserData(mLaser);
    this->GetPowers(mPowers);
    this->GetPlayerData(mPlayer);
    //确定当前状态
    //超过5个P点就尝试上线收点
    //if (mPowers.size() > 5)
    //{
    //	mState = GameState::COLLECT;
    //}else
    mState = GameState::NORMAL;
    switch (mState) {
        case GameState::NORMAL: {
            bool ok = false;
            double value[9][2];
            double maxValue = -99999999999.0;
            int moveKeyChoice = -1;
            double minEnemyDisSave = 999;
            bool useShift = false;
            bool useBomb = false;
            memset(value, 0, sizeof(value));
            //初次估价
            double minEnemyDis = 400.0;
            for (int low = 1; low >= 0; --low) {
                for (int movement = 0; movement < 9; ++movement) {
                    int dir = movement;
                    Vec2d newPos = fixupPos(Vec2d(mPlayer.pos.x + playerSpeed[low] * dx[dir],
                                                  mPlayer.pos.y + playerSpeed[low] * dy[dir]));
                    Object newPlayer = Object(newPos.x, newPos.y, mPlayer.size.x, mPlayer.size.y);
                    minEnemyDis = 400.0;
                    //P点距离估价
                    //double powerValue = 0;
                    double minPowerDis = 390400.0;
                    double y = -1;
                    for (auto &power : mPowers) {
                        double dis = (power.pos.x - newPos.x) * (power.pos.x - newPos.x) +
                                     (power.pos.y - newPos.y) * (power.pos.y - newPos.y);
                        //powerValue += dis;
                        if (dis < minPowerDis) {
                            minPowerDis = dis;
                            y = power.pos.y;
                        }
                        minPowerDis = std::min(minPowerDis, dis);
                    }
                    //if(!mPowers.empty())
                    //	powerValue /= mPowers.size();
                    if (y >= 250)
                        value[dir][low] += 350 * (390400.0 - minPowerDis) / 390400.0;
                    else
                        value[dir][low] += 150 * (390400.0 - minPowerDis) / 390400.0;
                    //地图位置估价
                    value[dir][low] += 10.0 * getMapValue(newPos);
                    //击破敌机估价
                    for (auto &i : mEnemy) {
                        minEnemyDis = std::min(minEnemyDis, std::abs(i.pos.x - newPos.x));
                    }
                    value[dir][low] += 80.0 * (400.0 - minEnemyDis) / 400.0;
                    //随机数
                    //if(indanger)
                    //	value[dir][low] += (double)(rand() % 100-50) / 100.0;
                }
            }
            //撞弹修正
            for (int low = 1; low >= 0; --low) {
                for (int movement = 0; movement < 9; ++movement) {
                    int dir = movement;
                    Vec2d newPos = fixupPos(Vec2d(mPlayer.pos.x + playerSpeed[low] * dx[dir],
                                                  mPlayer.pos.y + playerSpeed[low] * dy[dir]));
                    Object newPlayer = Object(newPos.x, newPos.y, mPlayer.size.x, mPlayer.size.y);
                    bool ishit = false;
                    for (auto bullet : mBullet) {
                        bullet.pos += bullet.delta;
                        if (hittest(bullet, newPlayer)) {
                            for (int r = 1; r < 9; ++r) {
                                Vec2d dirVec(dx[r], dy[r]);
                                //方向d的价值与方向d和子弹运动方向的夹角有关，越接近90°，价值越高
                                double dirvalue = 1 - abs(dirVec.dot(bullet.delta.unit()));
                                value[r][0] += 80.0 * dirvalue;//权重为80
                                value[r][1] += 160.0 * dirvalue;//权重为160
                            }
                            ishit = true;
                            break;
                        }
                    }
                    for (auto enemy : mEnemy) {
                        enemy.pos += enemy.delta;
                        if (hittest(enemy, newPlayer)) {
                            ishit = true;
                            break;
                        }
                    }
                    if (ishit)
                        value[dir][low] -= 9999999.0;
                }
            }
            //选择最高估价以及信息输出
            for (int low = 1; low >= 0; --low) {
                for (int movement = 0; movement < 9; ++movement) {
                    int dir = movement;
                    //std::cout << value[dir][low] << std::endl;
                    if (value[dir][low] > maxValue) {
                        maxValue = value[dir][low];
                        useShift = low;
                        moveKeyChoice = keyinfo[dir];
                        minEnemyDisSave = minEnemyDis;
                    }
                }
            }

            if (maxValue < 0)useBomb = true;
            if (!mLaser.empty())useBomb = true;
            if (mEnemy.size() <= 1 && mBullet.empty())
                KeyboardManager::sendKeyInfo(moveKeyChoice, useShift, frameCount % 2, useBomb);
            else
                KeyboardManager::sendKeyInfo(moveKeyChoice, useShift, minEnemyDisSave <= 100.0, useBomb);
            break;
        }
        default:
            break;
    }
}
