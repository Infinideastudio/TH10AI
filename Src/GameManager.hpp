#pragma once

#include <vector>
#include <memory>
#include <map>
#include "Vec2.hpp"
#include "Object.hpp"
#include "GameConnection.hpp"
#include <queue>

const double s2d2 = sqrt(2.0) / 2.0;
const double dx[9] = {0, 1.0, s2d2, 0, -s2d2, -1.0, -s2d2, 0, s2d2};
const double dy[9] = {0, 0, -s2d2, -1.0, -s2d2, 0, s2d2, 1.0, s2d2};
const double playerSpeed[2] = {4.5, 2.0};
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
    NodeSave() = default;
    NodeSave(int from_, bool shift_, double value_) {
        from = from_;
        shift = shift_;
        value = value_;
    }
};

class GameManager {
public:
    GameManager() : mState(GameState::NORMAL), mConnection(createGameConnection()) {}
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
    std::unique_ptr<GameConnection> mConnection;
    bool legalState(Node state);
    //估价效率过低，待修改
    double getValue(Node state);
    Vec2d fixupPos(const Vec2d &pos);
    bool hittest(Object &a, Object &b);
    //地图位置估价
    double getMapValue(Vec2d pos);
    double getDis(Vec2d point1, Vec2d point2) {
        return sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
    }
    double getSquareDis(Vec2d point1, Vec2d point2) {
        return (point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y);
    }
};