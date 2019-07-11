#pragma once

#include <map>
#include <queue>
#include "GameConnection.hpp"

const double s2d2 = sqrt(2.0) / 2.0;
const double dx[9] = {0, 1.0, s2d2, 0, -s2d2, -1.0, -s2d2, 0, s2d2};
const double dy[9] = {0, 0, -s2d2, -1.0, -s2d2, 0, s2d2, 1.0, s2d2};
const double dx4[5] = { 0, 1.0, 0, -1.0, 0};
const double dy4[5] = { 0, 0, -1.0, 0, 1.0 };
const double playerSpeed[2] = {4.5, 2.0};
const Vec2d ulCorner = Vec2d(-184, 32);
const Vec2d drCorner = Vec2d(184, 432);
const double eps = 1e-2;
//从高位到低位分别为上下左右
const int keyinfo[9] = {0x0, 0x1, 0x9, 0x8, 0xa, 0x2, 0x6, 0x4, 0x5};
const int keyinfo4[5] = { 0x0,0x1,0x8,0x2,0x4 };
enum class GameState {
    NORMAL,
    COLLECT,
    MOVE
};
struct Node {
    int8_t time;
    Vec2d pos;
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
    GameManager() : mState(GameState::NORMAL), mConnection(createGameConnection()) {}
    void update(unsigned long long frameCount,bool enabledMouse);
	void outputValueMap(const char* path);
    int getTimeline() noexcept;
private:
    std::map<Node, NodeSave> valueMap;
    std::queue<Node> bfsQueue;
    GameState mState;
    Player mPlayer {};
    std::vector<Object> mEnemy;
    std::vector<Object> mBullet;
    std::vector<Laser> mLaser;
    std::vector<Object> mPowers;
	Vec2d mMousePos;
	bool mouseMode;
    std::unique_ptr<GameConnection> mConnection;
    bool legalState(Node state) const noexcept;
    //估价效率过低，待修改
    double getValue(Node state) const noexcept;
    static Vec2d fixupPos(const Vec2d& pos) noexcept;
    static bool hitTest(const Object& a, const Object& b) noexcept;
	// Frame Advancing
	void updateEnemyLaserBoxes(const double ratio) noexcept;
	void updateBoardInformation(double ratio) noexcept;
    //地图位置估价
    static double getMapValue(Vec2d pos) noexcept;
};
