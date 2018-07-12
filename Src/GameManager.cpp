#include "GameManager.hpp"
#include "bmpCreater.hpp"
#include <future>

namespace {
    // 6 : 1048576 2: 128
    constexpr uint64_t compress(const Node& in) noexcept {
        return static_cast<uint64_t>(in.time) << 60 |
            static_cast<uint64_t>(in.pos.x * 1048576) << 30 | static_cast<uint64_t>(in.pos.y * 1048576);
    }

    constexpr const auto maxInvincibleTime = 240;

    constexpr const auto maxDepth = 4;

    constexpr const auto s2D2 = 0.70710678118654752440084436;

    constexpr const Vec2d direction[] = {
        {0.0, 0.0}, {1.0, 0.0}, {s2D2, -s2D2}, {0.0, -1.0}, {-s2D2, -s2D2}, {-1.0, 0.0}, {-s2D2, s2D2}, {0.0, 1.0},
        {s2D2, s2D2}
    };

    constexpr double playerSpeed[2] = {4.5, 2.0};

    constexpr Vec2d ulCorner{-200, 0}, drCorner{200, 480};

    const double eps = 1e-7;

    //从高位到低位分别为上下左右
    const int keyinfo[9] = {0x0, 0x1, 0x9, 0x8, 0xa, 0x2, 0x6, 0x4, 0x5};
}

GameManager::GameManager(): mConnection(createGameConnection()) {
    valueMap.set_empty_key(compress(Node(0, {0.0, 0.0})));
    valueMap.set_deleted_key(compress(Node(-1, {0.0, 0.0})));
}

Vec2d pointRotate(const Vec2d& target, const Vec2d& center, const double arc) noexcept {
    const auto x = (target.x - center.x) * cos(arc) - (target.y - center.y) * sin(arc);
    const auto y = (target.x - center.x) * sin(arc) + (target.y - center.y) * cos(arc);
    return center + Vec2d(x, y);
}

void GameManager::updateEnemyLaserBoxes() noexcept {
    //将激光的判定设为用AABB包起来那么大(gg,先这么写再慢慢改吧)
    for (auto& laser : mLaser) {
        const auto arc = laser.arc - 3.1415926 * 5.0 / 2.0;
        const auto ul = pointRotate(Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y), laser.pos, arc),
                   ur = pointRotate(Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y), laser.pos, arc),
                   dl = pointRotate(Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y + laser.size.y), laser.pos,
                                    arc),
                   dr = pointRotate(Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y + laser.size.y), laser.pos,
                                    arc);
        laser.pos = (ul + ur + dl + dr) / 4.0;
        const auto sizeX = std::max(std::max(ul.x, ur.x), std::max(dl.x, dr.x)) - std::min(
            std::min(ul.x, ur.x), std::min(dl.x, dr.x));
        const auto sizeY = std::max(std::max(ul.y, ur.y), std::max(dl.y, dr.y)) - std::min(
            std::min(ul.y, ur.y), std::min(dl.y, dr.y));
        laser.size = Vec2d(sizeX, sizeY);
    }
}

void GameManager::doValueMapOutput(const char* path) const noexcept {
    static Pixel map[480][400];
    memset(map, 0, sizeof(map)); // 设置背景为黑色
    double now = 0;
    for (auto i = 0; i < 400; ++i) {
        for (auto j = 0; j < 480; ++j) {
            now++;
            const Node state{0, Vec2d(i - 200, j)};
            if (legalState(state)) {
                constexpr auto k = 255.0 / 600.0, b = 140.0 * k;
                const auto value = k * getValue(state) + b;
                map[479 - j][i].g = std::min(255, std::max(0, static_cast<int>(value)));
                map[479 - j][i].r = std::min(255, std::max(0, 255 - static_cast<int>(value)));
            }
        }
    }
    generateBmp(map, {400, 480}, path);
}

void GameManager::outputValueMap(const char* path) noexcept {
    updateBoardInformation(1000.0);
    auto fork = *this;
    auto res = std::async(std::launch::async, [fork = std::move(fork), path]() { fork.doValueMapOutput(path); });
}

static int invincibleTime = 0;

void GameManager::updateBoardInformation(const double ratio) noexcept {
    mConnection->getPlayerData(mPlayer);
    mConnection->getEnemyData(mEnemy);
    mConnection->getEnemyBulletData(mBullet, mPlayer, ratio);
    mConnection->getEnemyLaserData(mLaser);
    mConnection->getPowers(mPowers);
    updateEnemyLaserBoxes();
}

void GameManager::pathEnumeration() noexcept {
    //BFS搜索maxDepth步，找到maxDepth步内价值最高的可到达位置。
    valueMap.clear_no_resize();
    bfsQueue.emplace(0, fixupPos(mPlayer.pos));
    while (!bfsQueue.empty()) {
        const auto now = bfsQueue.front();
        const auto nowData = valueMap[compress(now)];
        bfsQueue.pop();
        for (auto i = 0; i < 9; ++i) {
            for (auto j = 0; j <= 1; ++j) {
                const Node nex{now.time + 1, fixupPos(now.pos + direction[i] * playerSpeed[j])};
                if (!legalState(nex))continue;
                if (auto&& [node, success] = valueMap.insert({compress(nex), NodeSave()}); success) {
                    if (now.time == 0)
                        node->second = NodeSave(i, static_cast<bool>(j), getValue(nex));
                    else
                        node->second = NodeSave(nowData.from, nowData.shift, getValue(nex));
                    if (nex.time == maxDepth) bfsQueue.push(nex);
                }
            }
        }
    }
}

bool GameManager::evaluateBombUse() noexcept {
    auto useBomb = false;
    if (invincibleTime <= 0) {
        //1.被子弹打中
        for (const auto& bullet : mBullet) {
            if (hitTestBombChoice(bullet, mPlayer)) {
                useBomb = true;
                invincibleTime = maxInvincibleTime;
                break;
            }
        }
        //2.被体术
        for (const auto& enemy : mEnemy) {
            if (hitTestBombChoice(enemy, mPlayer)) {
                useBomb = true;
                invincibleTime = maxInvincibleTime;
                break;
            }
        }
        //3.被激光打中
        for (const auto& laser : mLaser) {
            if (hitTestBombChoice(laser, mPlayer)) {
                useBomb = true;
                invincibleTime = maxInvincibleTime;
                break;
            }
        }
    }
    return useBomb;
}

void GameManager::selectBestPath(int& moveKeyChoice, bool& useShift) noexcept {
    auto maxValue = -99999999999.0;
    for (auto&& item : valueMap) {
        if (item.first >> 60 == 0)continue;
        if (item.second.value - maxValue > eps) {
            maxValue = item.second.value;
            useShift = item.second.shift;
            moveKeyChoice = keyinfo[item.second.from];
        }
    }
}

void GameManager::update(const unsigned long long frameCount) noexcept {
    updateBoardInformation(static_cast<double>(maxDepth) * playerSpeed[0] + 15.0);
    if (invincibleTime == maxInvincibleTime - 60 && mPowers.empty()) { invincibleTime = 0; }
    if (invincibleTime > 0)invincibleTime--;

    pathEnumeration();
    //选择最高估价
    auto moveKeyChoice = -1;
    auto useShift = false;
    selectBestPath(moveKeyChoice, useShift);

    const auto useBomb = evaluateBombUse();
    //发送决策
    if (mEnemy.size() <= 1 && mBullet.empty())
        //跳过对话，间隔帧按Z
        mConnection->sendKeyInfo(moveKeyChoice, useShift, frameCount % 2, useBomb);
    else
        //正常进行游戏
        mConnection->sendKeyInfo(moveKeyChoice, useShift, true, useBomb);
}

//判断状态是否合法(某个位置能否到达)
bool GameManager::legalState(const Node& state) const noexcept {
    if (invincibleTime > 0)return true;
    for (auto bullet : mBullet) {
        bullet.pos += bullet.delta * state.time;
        if (hitTest(bullet, mPlayer)) { return false; }
    }
    for (auto enemy : mEnemy) {
        enemy.pos += enemy.delta * state.time;
        if (hitTest(enemy, mPlayer)) { return false; }
    }
    //for (auto laser : mLaser) {
    //	if (hitTest(laser, mPlayer)) { return false; }
    //}
    return true;
}

double GameManager::getThreatValue(const Vec2d& newPos) const noexcept {
    auto avgScore = 0.0, count = 0.0, avgScore2 = 0.0, count2 = 0.0;
    //敌机估价
    for (auto& enemy : mEnemy) {
        constexpr const Vec2d up(0, -1);
        const auto dis = distanceSqr(enemy.pos, newPos);
        auto selfDir = (newPos - enemy.pos).unit();
        //站的位置偏高，容易被弹幕封死。“从敌机指向自机的向量”和“从敌机指向正上方的向量”的夹角越大越安全，减分越少。
        if (enemy.pos.y <= 240) {
            avgScore -= selfDir.dot(up) + 1.0;
            count++;
        }
        //离敌机过进容易被发出的弹幕打死，也可能被体术。因此距离越近减分越多。
        if (dis <= 20000.0) {
            avgScore2 -= 1.0 - dis / 20000.0;
            count2++;
        }
    }
    if (count > 0) avgScore /= count;
    if (count2 > 0) avgScore2 /= count2;
    return 80.0 * avgScore + 80.0 * avgScore2;
}

double GameManager::getAttackValue(const Node& state) const noexcept {
    //子弹估价(和子弹运动方向夹角越大减分越少)
    if (invincibleTime > 0) return 0.0;
    auto avgScore = 0.0, count = 0.0;
    for (auto bullet : mBullet) {
        bullet.pos += bullet.delta * state.time;
        if (distanceSqr(bullet.pos, state.pos) <= 900) {
            count++;
            auto selfDir = (state.pos - bullet.pos).unit();
            const auto bulletDir = bullet.delta.unit();
            avgScore -= selfDir.dot(bulletDir) + 1.0;
        }
    }
    if (count > 0) avgScore /= count;
    return 70.0 * avgScore;
}

double GameManager::getKillValue(const Node& state) const noexcept {
    //击破敌机估价(站在敌机正下方加分)
    auto value = 0.0, minEnemyDis = 400.0;
    if (invincibleTime == 0) {
        for (auto& enemy : mEnemy) {
            auto dis = abs(enemy.pos.x + enemy.delta.x * state.time - state.pos.x);
            minEnemyDis = std::min(minEnemyDis, dis);
        }
        value += 80.0 * (400 - minEnemyDis) / 400;
    }
    return value;
}

double GameManager::getPowerValue(const Node& state) const noexcept {
    //收点估价，离点越近，价值越高   
    auto minPowerDis = 390400.0;
    for (auto& power : mPowers) {
        const auto newPowerPos = power.pos + power.delta * state.time;
        const auto dis = distanceSqr(newPowerPos, state.pos);
        if (dis < minPowerDis) { minPowerDis = dis; }
    }
    return (invincibleTime > 0 ? 480 : 180) * (390400.0 - minPowerDis) / 390400.0;
}

//对状态进行估价
double GameManager::getValue(const Node& state) const noexcept {
    return getPowerValue(state) + 80.0 * getMapValue(state.pos) + getKillValue(state) + getAttackValue(state) +
        getThreatValue(state.pos) - 0.1 * state.time;
}

//修正超出地图的坐标(主要是自机)
Vec2d GameManager::fixupPos(const Vec2d& pos) noexcept {
    auto res = pos;
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

//地图位置估价(站在地图偏下的位置加分)
double GameManager::getMapValue(const Vec2d& pos) noexcept {
    if (pos.y <= 100)
        return pos.y * 0.9 / 100;
    const auto dis = (abs(390 - pos.y) * (-10.0 / 290.0) + 100.0) / 100.0;
    const auto disx = (200 - abs(0 - pos.x)) / 200.0;
    return dis * 0.95 + disx * 0.05;
}
