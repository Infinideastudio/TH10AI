#pragma once

#include <vector>
#include <memory>
#include <map>
#include <queue>
#include <algorithm>
#include "Vec2.hpp"
#include "Object.hpp"
#include "Windows.hpp"

const double s2d2 = sqrt(2.0) / 2.0;
const double dx[9] = {0, 1.0, s2d2, 0, -s2d2, -1.0, -s2d2, 0, s2d2};
const double dy[9] = {0, 0, -s2d2, -1.0, -s2d2, 0, s2d2, 1.0, s2d2};
const double playerSpeed[2] = {9.0, 4.0};
const Vec2d ulCorner = Vec2d(-200, 0);
const Vec2d drCorner = Vec2d(200, 480);
const double eps = 1e-7;
//从高位到低位分别为上下左右
const int keyinfo[9] = {0x0, 0x1, 0x9, 0x8, 0xa, 0x2, 0x6, 0x4, 0x5};

enum class GameState {
    NORMAL,
    COLLECT,
    MOVE
};

struct Node {
    int8_t time;
    Vec2d pos;
    Node() { time = -1, pos = Vec2d(-1, -1); }
    Node(int8_t time_, Vec2d pos_) {
        time = time_;
        pos = pos_;
    }
};

bool operator<(const Node &lhs, const Node &rhs);

struct NodeSave {
    bool shift;
    int from;
    double value;
    NodeSave() {}
    NodeSave(int from_, bool shift_, double value_) {
        from = from_;
        shift = shift_;
        value = value_;
    }
};

class GameManager {
public:
    GameManager(HANDLE hprocess) {
        mHprocess = hprocess;
        mState = GameState::NORMAL;
    }
    void update(unsigned long long frameCount);
private:
    std::map<Node, NodeSave> valueMap;
    std::queue<Node> bfsQueue;
    GameState mState;
    Player mPlayer;
    std::vector<Object> mEnemy;
    std::vector<Object> mBullet;
    std::vector<Laser> mLaser;
    std::vector<Object> mPowers;
    HANDLE mHprocess;
    bool legalState(Node state) {
        Object newPlayer = Object(state.pos.x, state.pos.y, mPlayer.size.x, mPlayer.size.y);
        for (auto bullet : mBullet) {
            bullet.pos += bullet.delta * state.time;
            if (hittest(bullet, newPlayer)) {
                return false;
            }
        }
        for (auto enemy : mEnemy) {
            enemy.pos += enemy.delta * state.time;
            if (hittest(enemy, newPlayer)) {
                return false;
            }
        }
        return true;
    }
    //估价效率过低，待修改
    double getValue(Node state) {
        double value = 0.0;
        //初次估价
        double minEnemyDis = 400.0;
        Vec2d newPos = state.pos;
        Object newPlayer = Object(newPos.x, newPos.y, mPlayer.size.x, mPlayer.size.y);
        minEnemyDis = 400.0;
        //P点距离估价
        double minPowerDis = 390400.0;
        double y = -1;
        for (auto &power : mPowers) {
            Vec2d newPowerPos = power.pos + power.delta * state.time;
            double dis = getSquareDis(newPowerPos, newPos);
            if (dis < minPowerDis) {
                minPowerDis = dis;
                y = newPowerPos.y;
            }
        }
        if (y >= 250)
            value += 160 * (390400.0 - minPowerDis) / 390400.0;
        else
            value += 80 * (390400.0 - minPowerDis) / 390400.0;
        //地图位置估价
        value += 10.0 * getMapValue(newPos);
        //击破敌机估价
        for (auto &enemy : mEnemy) {
            double dis = abs(enemy.pos.x + enemy.delta.x * state.time - newPos.x);
            minEnemyDis = std::min(minEnemyDis, dis);
        }
        value += 80.0 * (400.0 - minEnemyDis) / 400.0;
        //value += rand() % 10/100.00;
        value -= 0.1 * state.time;
        return value;
    }
    Vec2d fixupPos(const Vec2d &pos) {
        Vec2d res = pos;
        if (res.x < ulCorner.x)res.x = ulCorner.x;
        if (res.y < ulCorner.y)res.y = ulCorner.y;
        if (res.x > drCorner.x)res.x = drCorner.x;
        if (res.y > drCorner.y)res.y = drCorner.y;
        return res;
    }
    bool hittest(Object &a, Object &b) {
        if (abs(a.pos.x - b.pos.x) <= ((a.size.x + b.size.x) / 2.0) + 5 &&
            abs(a.pos.y - b.pos.y) <= ((a.size.y + b.size.y) / 2.0) + 5)
            return true;
        return false;
    }
    //地图位置估价
    double getMapValue(Vec2d pos) {
        double dis = abs(390 - pos.y);
        double disx = abs(0 - pos.x) / 10;
        return ((410.0) - dis - disx) / (410.0);
        /*
        //最中心一块(-100,240)~(100,440)
        if (pos.x >= -100 && pos.x <= 100 && pos.y >= 240 && pos.y <= 440)return 1.00;
        //中间层(-180,100)~(180,480)
        if (pos.x >= -180 && pos.x <= 180 && pos.y >= 100 && pos.y <= 480)return 0.80;
        //外层
        */
        //return 0.60;
    }
    void GetPowers(std::vector<Object> &powers);
    void GetEnemyData(std::vector<Object> &enemy);
    void GetEnemyBulletData(std::vector<Object> &bullet, double maxRange);
    void GetPlayerData(Player &self);
    void GetEnemyLaserData(std::vector<Laser> &laser);
    double getDis(Vec2d point1, Vec2d point2) {
        return sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
    }
    double getSquareDis(Vec2d point1, Vec2d point2) {
        return (point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y);
    }
};