#pragma once

#include <memory>
#include <vector>
#include "Object.hpp"

struct GameConnection {
    virtual ~GameConnection() noexcept = default;
    virtual void GetPowers(std::vector<Object> &powers) noexcept = 0;
    virtual void GetEnemyData(std::vector<Object> &enemy) noexcept = 0;
    virtual void GetEnemyBulletData(std::vector<Object> &bullet, const Player &player, double maxRange) noexcept = 0;
    virtual void GetPlayerData(Player &self) noexcept = 0;
    virtual void GetEnemyLaserData(std::vector<Laser> &laser) noexcept = 0;
    virtual void sendKeyInfo(int dir,bool shift,bool z,bool x) noexcept = 0;
};

std::unique_ptr<GameConnection> createGameConnection();
