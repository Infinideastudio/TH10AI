#include "Windows.hpp"
#include "GameManager.hpp"
#include "bmpCreater.hpp"
#include "Vec2.hpp"
#include <iostream>
Vec2d pointRotate(Vec2d target, Vec2d center, double arc) {
    float _x, _y;
    _x = (target.x - center.x) * cos(arc) - (target.y - center.y) * sin(arc);
    _y = (target.x - center.x) * sin(arc) + (target.y - center.y) * cos(arc);
    return center + Vec2d(_x, _y);
}
void GameManager::outputValueMap(const char* path) {
    updateBoardInformation(99999.0);
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
                map[479 - j][i].g = std::min(255, std::max(0,(int)value));
                map[479 - j][i].r = std::min(255, std::max(0, 255-(int)value));
            }
        }
        //if (i % 100 == 0)std::cout << now / total * 100.0<<"%" << std::endl;
    }
    generateBmp((BYTE*)map, 400, 480, path);
}
void GameManager::updateEnemyLaserBoxes(const double ratio) noexcept {
    for (auto& laser : mLaser) {
        const auto arc = laser.arc - 3.1415926 * 5.0 / 2.0;
        const auto ul = pointRotate(Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y), laser.pos, arc),
            ur = pointRotate(Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y), laser.pos, arc),
            dl = pointRotate(Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y + laser.size.y), laser.pos,
                arc),
            dr = pointRotate(Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y + laser.size.y), laser.pos,
                arc);
        const auto s = (ul + ur)*0.5;
        const auto t = (dl + dr)*0.5;
        const auto delta = (t - s).unit()*5.0;
        double w = laser.size.x*0.5 + 2.0;
        double add = w / laser.size.y;
        for (double i = 0; i <= 1; i += add){
            double x = i * s.x + (1.0 - i)*t.x;
            double y = i * s.y + (1.0 - i)*t.y;
            if (distanceSqr(Vec2d(x, y), mPlayer.pos) <= ratio * ratio) {
                mBullet.emplace_back(x, y, w, w, delta.x, delta.y);
            }
        }
    }
}
void GameManager::updateBoardInformation(const double ratio) noexcept {
    mConnection->getPlayerData(mPlayer);
    mConnection->getEnemyData(mEnemy);
    mConnection->getEnemyBulletData(mBullet, mPlayer, ratio);
    mConnection->getEnemyLaserData(mLaser);
    mConnection->getPowers(mPowers);
    updateEnemyLaserBoxes(ratio);
}
int invincibleTime=0;
void GameManager::update(unsigned long long frameCount,bool enabledMouse) {
	mouseMode = enabledMouse;
	const int maxDepth = 7;
	const int maxInvincibleTime=240;
	updateBoardInformation((double)maxDepth * playerSpeed[0] + 15.0);
	mConnection->getMousePosition(mMousePos);
	mMousePos = fixupPos(mMousePos);
	if (invincibleTime == maxInvincibleTime-60 && mPowers.size() == 0){
        invincibleTime = 0;
    }
    if (invincibleTime > 0)invincibleTime--;
    //确定当前状态
    mState = GameState::NORMAL;
    switch (mState) {
    case GameState::NORMAL: {
        //BFS搜索maxDepth步，找到maxDepth步内价值最高的可到达位置。
        valueMap.erase(valueMap.begin(), valueMap.end());
        Node startState = Node(0, fixupPos(mPlayer.pos));
        valueMap[startState] = NodeSave(0, false, getValue(startState));
        bfsQueue.push(startState);
        while (!bfsQueue.empty()) {
            Node now = bfsQueue.front();
            NodeSave nowData = valueMap[now];
            bfsQueue.pop();
            if (now.time >= maxDepth)continue;
            for (int i = 0; i < 9; ++i) {
                for (int j = 0; j <= 1; ++j) {
                    if (now.time > 0&&nowData.shift != j)continue;
                    Node nex = Node(now.time + 1, fixupPos(
                        Vec2d(now.pos.x + dx[i] * playerSpeed[j], now.pos.y + dy[i] * playerSpeed[j])));
                    if (valueMap.find(nex) == valueMap.end()) {
                        if (!legalState(nex))continue;
                        if (now.time == 0)
                            valueMap[nex] = NodeSave(i, static_cast<bool>(j), getValue(nex));
                        else
                            valueMap[nex] = NodeSave(nowData.from, nowData.shift, getValue(nex));
                        bfsQueue.push(nex);
                    }
                }
            }
        }
        //选择最高估价
        bool haveNoChoice = true;
        double maxValue = -99999999999.0;
        int moveKeyChoice = -1;
        bool useShift = false;
        bool useBomb = false;
        NodeSave movement;
        //std::cout << valueMap.size() << std::endl;
        for (auto&& item : valueMap) {
            if (item.first.time == 0)continue;
            if (item.second.value - maxValue > doubleEqualEps) {
                haveNoChoice = false;
                maxValue = item.second.value;
                useShift = item.second.shift;
                moveKeyChoice = keyinfo[item.second.from];
                movement = item.second;
            }
        }
        //扔雷判断
        useBomb = false;
        if (mConnection->getPlayerStateInformation() == PlayerState::DYING){
            invincibleTime = maxInvincibleTime;
            useBomb = true;
        }
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
    //鼠标引导模式估价
    if (mouseMode)
    {
        double dis = distance(mMousePos, newPos);
        value += 150.0 * (sqrt(390400.0) - dis) / sqrt(390400.0);
        value -= 0.1 * state.time;
        return value;
    }
    minEnemyDis = 400.0;
    //收点估价，离点越近，价值越高   
    double minPowerDis = 390400.0;
    for (auto& power : mPowers) {
        Vec2d newPowerPos = power.pos + power.delta * state.time;
        double dis = distanceSqr(newPowerPos, newPos);
        if (dis < minPowerDis) {
            minPowerDis = dis;
        }
    }
    if (invincibleTime>0)value += 400 * (390400.0 - minPowerDis) / 390400.0;
    else value += 180 * (390400.0 - minPowerDis) / 390400.0;

    //地图位置估价(站在地图偏下的位置加分)
    value += 80.0 * getMapValue(newPos);
    //击破敌机估价(站在敌机正下方加分)
    if (invincibleTime == 0){
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
//决策时的碰撞检测
bool GameManager::hitTest(const Object& a, const Object& b) noexcept {
	const double hitTestEps = 0.05;//此值过小可能导致AI经常冲太高死，但从算法上改进比调大此值要好
    return abs(a.pos.x - b.pos.x) - ((a.size.x + b.size.x) / 2.0) <= hitTestEps &&
        abs(a.pos.y - b.pos.y) - ((a.size.y + b.size.y) / 2.0) <= hitTestEps;
}
//地图位置估价
double GameManager::getMapValue(Vec2d pos) noexcept {
    if (pos.y <= 100)
        return pos.y * 0.9 / 100;
    double dis = (abs(390 - pos.y) * (-10.0 / 290.0) + 100.0) / 100.0;
    double disx = (200 - abs(0 - pos.x)) / 200.0;
    return dis * 0.95 + disx * 0.05;
}

bool operator<(const Node& lhs, const Node& rhs) {
    if (lhs.time < rhs.time)return true;
    if (lhs.time > rhs.time)return false;
    if ((rhs.pos.x - lhs.pos.x) > eps)return true;
    if ((lhs.pos.x - rhs.pos.x) > eps)return false;
    if ((rhs.pos.y - lhs.pos.y) > eps)return true;
    return false;
}
int GameManager::getTimeline() noexcept{
    return mConnection->getTimeline();
}
