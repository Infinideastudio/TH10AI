#include "GameManager.hpp"
#include "bmpCreater.hpp"
#include <iostream>

namespace {
    constexpr uint64_t compress(const Node& in) noexcept {
        return static_cast<uint64_t>(in.time) << 60 |
            static_cast<uint64_t>(in.pos.x * 1048576) << 30 | static_cast<uint64_t>(in.pos.y * 1048576);
    }
}

GameManager::GameManager(): mState(GameState::NORMAL), mConnection(createGameConnection()) {
    valueMap.set_empty_key(compress(Node(0, {0.0, 0.0})));
    valueMap.set_deleted_key(compress(Node(-1, {0.0, 0.0})));
}

Vec2d pointRotate(const Vec2d target, const Vec2d center, const double arc) noexcept {
    const auto x = (target.x - center.x) * cos(arc) - (target.y - center.y) * sin(arc);
    const auto y = (target.x - center.x) * sin(arc) + (target.y - center.y) * cos(arc);
    return center + Vec2d(x, y);
}

void GameManager::updateEnemyLaserBoxes() {
    //将激光的判定设为用AABB包起来那么大(gg,先这么写再慢慢改吧)
    for (Laser& laser : mLaser) {
        Vec2d ul = Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y);
        Vec2d ur = Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y);
        Vec2d dl = Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y + laser.size.y);
        Vec2d dr = Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y + laser.size.y);
        double arc = laser.arc - 3.1415926 * 5.0 / 2.0;
        ul = pointRotate(ul, laser.pos, arc);
        ur = pointRotate(ur, laser.pos, arc);
        dl = pointRotate(dl, laser.pos, arc);
        dr = pointRotate(dr, laser.pos, arc);
        laser.pos = (ul + ur + dl + dr) / 4.0;
        double sizeX = std::max(std::max(ul.x, ur.x), std::max(dl.x, dr.x)) - std::min(
            std::min(ul.x, ur.x), std::min(dl.x, dr.x));
        double sizeY = std::max(std::max(ul.y, ur.y), std::max(dl.y, dr.y)) - std::min(
            std::min(ul.y, ur.y), std::min(dl.y, dr.y));
        laser.size = Vec2d(sizeX, sizeY);
    }
}

void GameManager::outputValueMap(const char* path) {
    updateBoardInformation(1000.0);
    static Pixel map[480][400];
    memset(map, 0, sizeof(map)); // 设置背景为黑色
    double total = 400 * 480;
    double now = 0;
    for (int i = 0; i < 400; ++i) {
        for (int j = 0; j < 480; ++j) {
            now++;
            Node state = Node(0, Vec2d(i - 200, j));
            if (legalState(state)) {
                double value = getValue(state);
                double k = 255.0 / 600.0, b = 140.0 * k;
                value = k * value + b;
                map[479 - j][i].g = std::min(255, std::max(0, (int)value));
                map[479 - j][i].r = std::min(255, std::max(0, 255 - (int)value));
            }
        }
        //if (i % 100 == 0)std::cout << now / total * 100.0<<"%" << std::endl;
    }
    generateBmp(map, 400, 480, path);
}

static int invincibleTime = 0;

void GameManager::updateBoardInformation(const double ratio) {
    mConnection->GetPlayerData(mPlayer);
    mConnection->GetEnemyData(mEnemy);
    mConnection->GetEnemyBulletData(mBullet, mPlayer, ratio);
    mConnection->GetEnemyLaserData(mLaser);
    mConnection->GetPowers(mPowers);
    updateEnemyLaserBoxes();
}

void GameManager::update(unsigned long long frameCount) {
    const int maxInvincibleTime = 240;
    const int maxDepth = 4;
    updateBoardInformation(static_cast<double>(maxDepth) * playerSpeed[0] + 15.0);
    if (invincibleTime == maxInvincibleTime - 60 && mPowers.empty()) { invincibleTime = 0; }
    if (invincibleTime > 0)invincibleTime--;
    //确定当前状态
    mState = GameState::NORMAL;
    switch (mState) {
    case GameState::NORMAL: {
        //BFS搜索maxDepth步，找到maxDepth步内价值最高的可到达位置。
        valueMap.clear_no_resize();
        Node startState = Node(0, fixupPos(mPlayer.pos));
        valueMap[compress(startState)] = NodeSave(0, false, getValue(startState));
        bfsQueue.push(startState);
        while (!bfsQueue.empty()) {
            const auto now = bfsQueue.front();
            const auto nowData = valueMap[compress(now)];
            bfsQueue.pop();
            if (now.time >= maxDepth)continue;
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j <= 1; ++j) {
                    const Node nex{now.time + 1, fixupPos(now.pos + Vec2d{dx[i], dy[i]} * playerSpeed[j])};
                    if (!legalState(nex))continue;
                    if (auto&& [node, success] = valueMap.insert({compress(nex), NodeSave()}); success) {
                        if (now.time == 0)
                            node->second = NodeSave(i, static_cast<bool>(j), getValue(nex));
                        else
                            node->second = NodeSave(nowData.from, nowData.shift, getValue(nex));
                        bfsQueue.push(nex);
                    }
                }
            }
        }
        std::cout << "ValueMap:" << valueMap.size() << std::endl;

        //选择最高估价
        bool haveNoChoice = true;
        double maxValue = -99999999999.0;
        int moveKeyChoice = -1;
        bool useShift = false;
        bool useBomb = false;
        NodeSave movement;
        for (auto&& item : valueMap) {
            if (item.first >> 60 == 0)continue;
            if (item.second.value - maxValue > eps) {
                haveNoChoice = false;
                maxValue = item.second.value;
                useShift = item.second.shift;
                moveKeyChoice = keyinfo[item.second.from];
                movement = item.second;
            }
        }
        //扔雷判断
        useBomb = false;
        Object newPlayer = Object(mPlayer.pos.x, mPlayer.pos.y, mPlayer.size.x, mPlayer.size.y);
        //1.被子弹打中
        if (invincibleTime > 0)useBomb = false;
        else {
            for (auto bullet : mBullet) {
                if (hitTestBombChoice(bullet, newPlayer)) {
                    useBomb = true;
                    invincibleTime = maxInvincibleTime;
                    break;
                }
            }
            //2.被体术
            for (auto enemy : mEnemy) {
                if (hitTestBombChoice(enemy, newPlayer)) {
                    useBomb = true;
                    invincibleTime = maxInvincibleTime;
                    break;
                }
            }
            //3.被激光打中
            for (auto laser : mLaser) {
                if (hitTestBombChoice(laser, newPlayer)) {
                    useBomb = true;
                    invincibleTime = maxInvincibleTime;
                    break;
                }
            }
        }
        //if (!mLaser.empty())useBomb = true;
        //发送决策
        if (mEnemy.size() <= 1 && mBullet.empty())
            //跳过对话，间隔帧按Z
            mConnection->sendKeyInfo(moveKeyChoice, useShift, frameCount % 2, useBomb);
        else
            //正常进行游戏
            mConnection->sendKeyInfo(moveKeyChoice, useShift, true, useBomb);
        break;
    }
    default:
        break;
    }
}

//判断状态是否合法(某个位置能否到达)
bool GameManager::legalState(Node state) const noexcept {
    if (invincibleTime > 0)return true;
    Object newPlayer = Object(state.pos.x, state.pos.y, mPlayer.size.x, mPlayer.size.y);
    for (auto bullet : mBullet) {
        bullet.pos += bullet.delta * state.time;
        if (hitTest(bullet, newPlayer)) { return false; }
    }
    for (auto enemy : mEnemy) {
        enemy.pos += enemy.delta * state.time;
        if (hitTest(enemy, newPlayer)) { return false; }
    }
    //for (auto laser : mLaser) {
    //	if (hitTest(laser, newPlayer)) { return false; }
    //}
    return true;
}

//对状态进行估价
double GameManager::getValue(Node state) const noexcept {
    double value = 0.0;
    double minEnemyDis = 400.0;
    Vec2d newPos = state.pos;
    Object newPlayer = Object(newPos.x, newPos.y, mPlayer.size.x, mPlayer.size.y);
    minEnemyDis = 400.0;
    //收点估价，离点越近，价值越高   
    double minPowerDis = 390400.0;
    for (auto& power : mPowers) {
        Vec2d newPowerPos = power.pos + power.delta * state.time;
        double dis = distanceSqr(newPowerPos, newPos);
        if (dis < minPowerDis) { minPowerDis = dis; }
    }
    if (invincibleTime > 0)value += 400 * (390400.0 - minPowerDis) / 390400.0;
    else value += 180 * (390400.0 - minPowerDis) / 390400.0;

    //地图位置估价(站在地图偏下的位置加分)
    value += 80.0 * getMapValue(newPos);
    //击破敌机估价(站在敌机正下方加分)
    if (invincibleTime == 0) {
        for (auto& enemy : mEnemy) {
            double dis = abs(enemy.pos.x + enemy.delta.x * state.time - newPos.x);
            minEnemyDis = std::min(minEnemyDis, dis);
        }
        value += 80.0 * (400 - minEnemyDis) / 400;
    }
    //子弹估价(和子弹运动方向夹角越大减分越少)
    double avgScore = 0;
    double count = 0;
    for (auto bullet : mBullet) {
        bullet.pos += bullet.delta * state.time;
        if (distanceSqr(bullet.pos, newPos) <= 900) {
            count++;
            Vec2d selfDir = (newPos - bullet.pos).unit();
            Vec2d bulletDir = bullet.delta.unit();
            //该位置的价值与该位置到子弹的连线和子弹运动方向的夹角有关，夹角越大，减分越少
            double dirvalue = selfDir.dot(bulletDir);
            dirvalue += 1;
            avgScore -= dirvalue;
        }
    }
    if (invincibleTime == 0 && count > 0) {
        avgScore /= count;
        value += 70.0 * avgScore;
    }
    avgScore = 0;
    count = 0;
    double count2 = 0;
    double avgScore2 = 0;
    //敌机估价
    for (auto& enemy : mEnemy) {
        double dis = distanceSqr(enemy.pos, newPos);
        Vec2d selfDir = (newPos - enemy.pos).unit();
        Vec2d up(0, -1);
        double dirvalue = selfDir.dot(up);
        //站的位置偏高，容易被弹幕封死。“从敌机指向自机的向量”和“从敌机指向正上方的向量”的夹角越大越安全，减分越少。
        if (enemy.pos.y <= 240) {
            dirvalue += 1;
            avgScore -= dirvalue;
            count++;
        }
        //离敌机过进容易被发出的弹幕打死，也可能被体术。因此距离越近减分越多。
        if (dis <= 20000.0) {
            avgScore2 -= 1.0 - (dis / 20000.0);
            count2++;
        }
    }
    if (count > 0) {
        avgScore /= count;
        value += 80.0 * avgScore;
    }
    if (count2 > 0) {
        avgScore2 /= count2;
        value += 80.0 * avgScore2;
    }
    //value += rand() % 10/100.00;
    value -= 0.1 * state.time;
    return value;
}

//修正超出地图的坐标(主要是自机)
Vec2d GameManager::fixupPos(const Vec2d& pos) noexcept {
    Vec2d res = pos;
    if (res.x < ulCorner.x)res.x = ulCorner.x;
    if (res.y < ulCorner.y)res.y = ulCorner.y;
    if (res.x > drCorner.x)res.x = drCorner.x;
    if (res.y > drCorner.y)res.y = drCorner.y;
    return res;
}

//扔雷时的碰撞检测，误差为1.0
bool GameManager::hitTestBombChoice(const Object& a, const Object& b) noexcept {
    return abs(a.pos.x - b.pos.x) - ((a.size.x + b.size.x) / 2.0) <= 1.0 &&
        abs(a.pos.y - b.pos.y) - ((a.size.y + b.size.y) / 2.0) <= 1.0;
}

//决策时的碰撞检测，将判定范围增加4.5后检测。请不要随意修改这个值。
//判定过大，AI底力下降。判定过小，AI更容易移动到极其接近子弹的地方，由于视野的局限性，容易撞弹。
bool GameManager::hitTest(const Object& a, const Object& b) noexcept {
    return abs(a.pos.x - b.pos.x) - ((a.size.x + b.size.x) / 2.0) <= 4.5 &&
        abs(a.pos.y - b.pos.y) - ((a.size.y + b.size.y) / 2.0) <= 4.5;
}

//地图位置估价
double GameManager::getMapValue(Vec2d pos) noexcept {
    if (pos.y <= 100)
        return pos.y * 0.9 / 100;
    double dis = (abs(390 - pos.y) * (-10.0 / 290.0) + 100.0) / 100.0;
    double disx = (200 - abs(0 - pos.x)) / 200.0;
    return dis * 0.95 + disx * 0.05;
}
