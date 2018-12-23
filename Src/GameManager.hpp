#pragma once

#include <map>
#include <queue>
#include "GameConnection.hpp"

<<<<<<< HEAD
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
=======
struct Node {
    int8_t time;
    Vec2d pos;
    Node() = default;
    constexpr Node(const int8_t iTime, const Vec2d& iPos) noexcept : time(iTime), pos(iPos) {}
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
};

bool operator<(const Node& lhs, const Node& rhs);
struct NodeSave {
    bool shift;
    int from;
    double value;
    NodeSave() = default;
    constexpr NodeSave(const int iFrom, const bool iShift, const double iValue) noexcept :
        shift(iShift), from(iFrom), value(iValue) {}
};
class GameManager {
public:
<<<<<<< HEAD
    GameManager() : mState(GameState::NORMAL), mConnection(createGameConnection()) {}
    void update(unsigned long long frameCount,bool enabledMouse);
	void outputValueMap(const char* path);
private:
    std::map<Node, NodeSave> valueMap;
    std::queue<Node> bfsQueue;
    GameState mState;
    Player mPlayer {};
=======
    GameManager();
    void outputValueMap(const char* path) noexcept;
    void update(unsigned long long frameCount) noexcept;
private:
    google::dense_hash_map<uint64_t, NodeSave> valueMap{5500};
    std::queue<Node> bfsQueue;
    Player mPlayer{};
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    std::vector<Object> mEnemy;
    std::vector<Object> mBullet;
    std::vector<Laser> mLaser;
    std::vector<Object> mPowers;
<<<<<<< HEAD
	Vec2d mMousePos;
	bool mouseMode;
    std::unique_ptr<GameConnection> mConnection;
    bool legalState(Node state) const noexcept;
    //估价效率过低，待修改
    double getValue(Node state) const noexcept;
    static Vec2d fixupPos(const Vec2d& pos) noexcept;
    static bool hitTest(const Object& a, const Object& b) noexcept;
	// Frame Advancing
	void updateEnemyLaserBoxes() noexcept;
	void updateBoardInformation(double ratio) noexcept;
    //地图位置估价
    static double getMapValue(Vec2d pos) noexcept;
=======
    std::shared_ptr<GameConnection> mConnection;
    // Frame Advancing
    void updateEnemyLaserBoxes() noexcept;
    void updateBoardInformation(double ratio) noexcept;
    // ValueMap Capture
    void doValueMapOutput(const char* path) const noexcept;
    // Action Evaluation
    void pathEnumeration() noexcept;
    bool evaluateBombUse() noexcept;
    bool legalState(const Node& state) const noexcept;
    void selectBestPath(int& moveKeyChoice, bool& useShift) noexcept;
    // Weight Evaluation
    double getValue(const Node& state) const noexcept;
    double getThreatValue(const Vec2d& newPos) const noexcept;
    double getAttackValue(const Node& state) const noexcept;
    double getKillValue(const Node& state) const noexcept;
    double getPowerValue(const Node& state) const noexcept;
    static Vec2d fixupPos(const Vec2d& pos) noexcept;
    static bool hitTest(const Object& a, const Object& b) noexcept;
    static bool hitTestBombChoice(const Object& a, const Object& b) noexcept;
    static double getMapValue(const Vec2d& pos) noexcept;
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
};
