#include "GameManager.hpp"
#include "KeyboardManager.hpp"

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
        ReadProcessMemory(mHprocess, (LPCVOID)(ebp + 0x2C), &eax, 4, &nbr);
        if (eax != 0) {
            float x, y;
            ReadProcessMemory(mHprocess, (LPCVOID)(ebp - 0x4), &x, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID)(ebp), &y, 4, &nbr);
            powers.emplace_back(x, y, 6, 6);
        }
        ebp += 0x3F0;
    }
}

void GameManager::GetEnemyData(std::vector<Object> &enemy) {
    DWORD nbr;
    int base, obj_base, obj_addr, obj_next;
    enemy.clear();
    ReadProcessMemory(mHprocess, (LPCVOID) 0x00477704, &base, 4, &nbr);
    if (base == NULL) {
        return;
    }
    ReadProcessMemory(mHprocess, (LPCVOID)(base + 0x58), &obj_base, 4, &nbr);
    if (obj_base != NULL) {
        while (true) {
            ReadProcessMemory(mHprocess, (LPCVOID) obj_base, &obj_addr, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID)(obj_base + 4), &obj_next, 4, &nbr);
            obj_addr += 0x103C;
            unsigned int t;
            ReadProcessMemory(mHprocess, (LPCVOID)(obj_addr + 0x1444), &t, 4, &nbr);
            if (!(t & 0x40)) {
                ReadProcessMemory(mHprocess, (LPCVOID)(obj_addr + 0x1444), &t, 4, &nbr);
                if (!(t & 0x12)) {
                    float x, y, w, h;
                    ReadProcessMemory(mHprocess, (LPCVOID)(obj_addr + 0x2C), &x, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID)(obj_addr + 0x30), &y, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID)(obj_addr + 0xB8), &w, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID)(obj_addr + 0xBC), &h, 4, &nbr);

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

void GameManager::GetEnemyBulletData(std::vector<Object> &bullet, double maxRange) {
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
        ReadProcessMemory(mHprocess, (LPCVOID)(edi + 0x46), &bp, 4, &nbr);
        bp = bp & 0x0000FFFF;
        if (bp) {
            int eax;
            ReadProcessMemory(mHprocess, (LPCVOID)(0x477810), &eax, 4, &nbr);
            if (eax != NULL) {
                ReadProcessMemory(mHprocess, (LPCVOID)(eax + 0x58), &eax, 4, &nbr);
                if (!(eax & 0x00000400)) {
                    float x, y, w, h, dx, dy;
                    ReadProcessMemory(mHprocess, (LPCVOID)(ebx + 0x3C0), &dx, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID)(ebx + 0x3C4), &dy, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID)(ebx + 0x3B4), &x, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID)(ebx + 0x3B8), &y, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID)(ebx + 0x3F0), &w, 4, &nbr);
                    ReadProcessMemory(mHprocess, (LPCVOID)(ebx + 0x3F4), &h, 4, &nbr);
                    //为了效率，只考虑可能会碰到的子弹
                    if (getSquareDis(Vec2d(x, y), mPlayer.pos) <= maxRange * maxRange)
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
    ReadProcessMemory(mHprocess, (LPCVOID)(obj_base + 0x3C0), &x, 4, &nbr);
    ReadProcessMemory(mHprocess, (LPCVOID)(obj_base + 0x3C4), &y, 4, &nbr);
    self.pos.x = x;
    self.pos.y = y;
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
    ReadProcessMemory(mHprocess, (LPCVOID)(base + 0x18), &esi, 4, &nbr);
    if (esi != NULL) {
        while (true) {
            ReadProcessMemory(mHprocess, (LPCVOID)(esi + 0x8), &ebx, 4, &nbr);
            float x, y, h, w, arc;
            ReadProcessMemory(mHprocess, (LPCVOID)(esi + 0x24), &x, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID)(esi + 0x28), &y, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID)(esi + 0x3C), &arc, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID)(esi + 0x40), &h, 4, &nbr);
            ReadProcessMemory(mHprocess, (LPCVOID)(esi + 0x44), &w, 4, &nbr);
            laser.emplace_back(x, y, w / 2.0f, h, arc);
            if (ebx == NULL) {
                break;
            }
            esi = ebx;
        }
    }
}

void GameManager::update(unsigned long long frameCount) {
    const int maxDepth = 4;
    this->GetPlayerData(mPlayer);
    this->GetEnemyData(mEnemy);
    this->GetEnemyBulletData(mBullet, (double) maxDepth * playerSpeed[0] + 15.0);
    this->GetEnemyLaserData(mLaser);
    this->GetPowers(mPowers);

    //确定当前状态
    //超过5个P点就尝试上线收点
    //if (mPowers.size() > 5)
    //{
    //	mState = GameState::COLLECT;
    //}else
    mState = GameState::NORMAL;
    switch (mState) {
        case GameState::NORMAL: {
            //bool ok = false;
            //double value[9][2];
            valueMap.erase(valueMap.begin(), valueMap.end());
            Node startState = Node(0, fixupPos(mPlayer.pos));
            valueMap[startState] = NodeSave(0, 0, getValue(startState));
            bfsQueue.push(startState);
            while (!bfsQueue.empty()) {
                Node now = bfsQueue.front();
                NodeSave nowData = valueMap[now];
                bfsQueue.pop();
                if (now.time >= maxDepth)continue;
                for (int i = 0; i < 9; ++i) {
                    for (int j = 0; j <= 1; ++j) {
                        Node nex = Node(now.time + 1, fixupPos(
                                Vec2d(now.pos.x + dx[i] * playerSpeed[j], now.pos.y + dy[i] * playerSpeed[j])));
                        if (valueMap.find(nex) == valueMap.end()) {
                            if (!legalState(nex))continue;
                            if (now.time == 0)
                                valueMap[nex] = NodeSave(i, j, getValue(nex));
                            else
                                valueMap[nex] = NodeSave(nowData.from, nowData.shift, getValue(nex));
                            bfsQueue.push(nex);
                        }
                    }
                }
            }
            //选择最高估价以及信息输出
            //printf("%d\n", valueMap.size());
            bool haveNoChoice = true;
            double maxValue = -99999999999.0;
            int moveKeyChoice = -1;
            bool useShift = false;
            bool useBomb = false;
            for (auto &&item:valueMap) {
                //printf("%d\n", item.second.value);
                if (item.first.time == 0)continue;
                //printf("%.3lf %.3lf %.3lf\n",item.first.pos.x,item.first.pos.y,item.second.value);
                if (item.second.value - maxValue > eps) {
                    haveNoChoice = false;
                    maxValue = item.second.value;
                    useShift = item.second.shift;
                    moveKeyChoice = keyinfo[item.second.from];
                }
            }
            //std::cout<<"value:" << maxValue << std::endl;
            if (haveNoChoice)useBomb = true;
            if (mLaser.size() > 0)useBomb = true;
            if (mEnemy.size() <= 1 && mBullet.size() == 0)
                KeyboardManager::sendKeyInfo(moveKeyChoice, useShift, frameCount % 2, useBomb);
            else
                KeyboardManager::sendKeyInfo(moveKeyChoice, useShift, true, useBomb);
            break;
        }
        default:
            break;
    }
}

bool operator<(const Node &lhs, const Node &rhs) {
    if (lhs.time < rhs.time)return true;
    if (lhs.time > rhs.time)return false;
    if ((rhs.pos.x - lhs.pos.x) > eps)return true;
    if ((lhs.pos.x - rhs.pos.x) > eps)return false;
    if ((rhs.pos.y - lhs.pos.y) > eps)return true;
    return false;
}
