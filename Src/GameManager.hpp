#pragma once

#include <queue>
#include "sparsehash/dense_hash_map"
#include "GameConnection.hpp"

struct Node {
    int8_t time;
    Vec2d pos;
    Node() = default;
    constexpr Node(const int8_t iTime, const Vec2d& iPos) noexcept : time(iTime), pos(iPos) {}
};

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
    GameManager();
    void outputValueMap(const char* path) noexcept;
    void update(unsigned long long frameCount) noexcept;
private:
    google::dense_hash_map<uint64_t, NodeSave> valueMap{5500};
    std::queue<Node> bfsQueue;
    Player mPlayer{};
    std::vector<Object> mEnemy;
    std::vector<Object> mBullet;
    std::vector<Laser> mLaser;
    std::vector<Object> mPowers;
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
};
