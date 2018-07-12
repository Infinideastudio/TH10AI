#pragma once

#include <queue>
#include "sparsehash/dense_hash_map"
#include "GameConnection.hpp"

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
    Node() = default;
    constexpr Node(int8_t time_, Vec2d pos_) noexcept : time(time_), pos(pos_) {}
};

bool operator<(const Node& lhs, const Node& rhs);

struct NodeSave {
    bool shift;
    int from;
    double value;
    NodeSave() = default;
    constexpr NodeSave(int from_, bool shift_, double value_) noexcept : shift(shift_), from(from_), value(value_) {}
};

class GameManager {
public:
    GameManager();
    void update(unsigned long long frameCount);
	void outputValueMap(const char* path);
private:
    google::dense_hash_map<uint64_t, NodeSave> valueMap { 5500 };
    std::queue<Node> bfsQueue;
    GameState mState;
    Player mPlayer{};
    std::vector<Object> mEnemy;
    std::vector<Object> mBullet;
    std::vector<Laser> mLaser;
    std::vector<Object> mPowers;
    std::unique_ptr<GameConnection> mConnection;
    bool legalState(Node state) const noexcept;
    //估价效率过低，待修改
    double getValue(Node state) const noexcept;
    static Vec2d fixupPos(const Vec2d& pos) noexcept;
    static bool hitTest(const Object& a, const Object& b) noexcept;
    static bool hitTestBombChoice(const Object& a, const Object& b) noexcept;
    //地图位置估价
    static double getMapValue(Vec2d pos) noexcept;
};
