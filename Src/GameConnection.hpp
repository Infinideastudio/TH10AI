#pragma once

#include <memory>
#include <vector>
#include "Object.hpp"

struct GameConnection {
    virtual ~GameConnection() noexcept = default;
    virtual void getPowers(std::vector<Object>& powers) noexcept = 0;
    virtual void getEnemyData(std::vector<Object>& enemy) noexcept = 0;
    virtual void getEnemyBulletData(std::vector<Object>& bullet, const Player& player, double maxRange) noexcept = 0;
    virtual void getPlayerData(Player& self) noexcept = 0;
    virtual void getEnemyLaserData(std::vector<Laser>& laser) noexcept = 0;
    virtual void sendKeyInfo(int dir, bool shift, bool z, bool x) noexcept = 0;
};

std::unique_ptr<GameConnection> createGameConnection();
