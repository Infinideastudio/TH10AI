#pragma once

#include <vector>
#include <memory>
#include "Vec2.hpp"
#include "Object.hpp"
#include "Windows.hpp"
#include <algorithm>

const double s2d2 = sqrt(2.0) / 2.0;
const double dx[9] = {0, 1.0, s2d2, 0, -s2d2, -1.0, -s2d2, 0, s2d2};
const double dy[9] = {0, 0, -s2d2, -1.0, -s2d2, 0, s2d2, 1.0, s2d2};
const double playerSpeed[2] = {9.0, 4.0};
const Vec2d ulCorner = Vec2d(-200, 0);
const Vec2d drCorner = Vec2d(200, 480);
//从高位到低位分别为上下左右
const int keyinfo[9] = {0x0, 0x1, 0x9, 0x8, 0xa, 0x2, 0x6, 0x4, 0x5};
enum class GameState {
    NORMAL,
    COLLECT,
    MOVE
};

class GameManager {
public:
    explicit GameManager(HANDLE hprocess) {
        mHprocess = hprocess;
        mState = GameState::NORMAL;
    }

    void update(unsigned long long frameCount);
private:
    GameState mState;
    Player mPlayer;
    HANDLE mHprocess;
    std::vector<Object> mEnemy;
    std::vector<Object> mBullet;
    std::vector<Laser> mLaser;
    std::vector<Object> mPowers;
    Vec2d fixupPos(const Vec2d &pos) {
        Vec2d res = pos;
        if (res.x < ulCorner.x)res.x = ulCorner.x;
        if (res.y < ulCorner.y)res.y = ulCorner.y;
        if (res.x > drCorner.x)res.x = drCorner.x;
        if (res.y > drCorner.y)res.y = drCorner.y;
        return res;
    }

    bool hittest(Object &a, Object &b) {
        return abs(a.pos.x - b.pos.x) <= ((a.size.x + b.size.x) / 2.0) + 3 &&
        abs(a.pos.y - b.pos.y) <= ((a.size.y + b.size.y) / 2.0) + 3;
    }
    //地图位置估价
    double getMapValue(Vec2d pos) {
        double dis = abs(390 - pos.y);
        return ((390.0) - dis) / (390.0);
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
    void GetEnemyBulletData(std::vector<Object> &bullet);
    void GetPlayerData(Player &self);
    void GetEnemyLaserData(std::vector<Laser> &laser);
};