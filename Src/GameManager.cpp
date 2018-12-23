#include "GameManager.hpp"
#include "bmpCreater.hpp"
<<<<<<< HEAD
#include <iostream>
Vec2d pointRotate(Vec2d target, Vec2d center, double arc) {
	float _x, _y;
	_x = (target.x - center.x) * cos(arc) - (target.y - center.y) * sin(arc);
	_y = (target.x - center.x) * sin(arc) + (target.y - center.y) * cos(arc);
	return center + Vec2d(_x, _y);
}
void GameManager::outputValueMap(const char* path) {
	updateBoardInformation(99999.0);
=======
#include <future>

namespace {
    // 6 : 1048576 2: 128
    constexpr uint64_t compress(const Node& in) noexcept {
        return static_cast<uint64_t>(in.time) << 60 |
            static_cast<uint64_t>(in.pos.x * 1048576) << 30 | static_cast<uint64_t>(in.pos.y * 1048576);
    }

    constexpr const auto maxInvincibleTime = 240;

    constexpr const auto maxDepth = 4;

    constexpr const auto s2D2 = 0.70710678118654752440084436;

    constexpr const Vec2d direction[] = {
        {0.0, 0.0}, {1.0, 0.0}, {s2D2, -s2D2}, {0.0, -1.0}, {-s2D2, -s2D2}, {-1.0, 0.0}, {-s2D2, s2D2}, {0.0, 1.0},
        {s2D2, s2D2}
    };

    constexpr double playerSpeed[2] = {4.5, 2.0};

    constexpr Vec2d ulCorner{-200, 0}, drCorner{200, 480};

    const double eps = 1e-7;

    //从高位到低位分别为上下左右
    const int keyinfo[9] = {0x0, 0x1, 0x9, 0x8, 0xa, 0x2, 0x6, 0x4, 0x5};
}

GameManager::GameManager(): mConnection(createGameConnection()) {
    valueMap.set_empty_key(compress(Node(0, {0.0, 0.0})));
    valueMap.set_deleted_key(compress(Node(-1, {0.0, 0.0})));
}

Vec2d pointRotate(const Vec2d& target, const Vec2d& center, const double arc) noexcept {
    const auto x = (target.x - center.x) * cos(arc) - (target.y - center.y) * sin(arc);
    const auto y = (target.x - center.x) * sin(arc) + (target.y - center.y) * cos(arc);
    return center + Vec2d(x, y);
}

void GameManager::updateEnemyLaserBoxes() noexcept {
    //将激光的判定设为用AABB包起来那么大(gg,先这么写再慢慢改吧)
    for (auto& laser : mLaser) {
        const auto arc = laser.arc - 3.1415926 * 5.0 / 2.0;
        const auto ul = pointRotate(Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y), laser.pos, arc),
                   ur = pointRotate(Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y), laser.pos, arc),
                   dl = pointRotate(Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y + laser.size.y), laser.pos,
                                    arc),
                   dr = pointRotate(Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y + laser.size.y), laser.pos,
                                    arc);
        laser.pos = (ul + ur + dl + dr) / 4.0;
        const auto sizeX = std::max(std::max(ul.x, ur.x), std::max(dl.x, dr.x)) - std::min(
            std::min(ul.x, ur.x), std::min(dl.x, dr.x));
        const auto sizeY = std::max(std::max(ul.y, ur.y), std::max(dl.y, dr.y)) - std::min(
            std::min(ul.y, ur.y), std::min(dl.y, dr.y));
        laser.size = Vec2d(sizeX, sizeY);
    }
}

void GameManager::doValueMapOutput(const char* path) const noexcept {
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    static Pixel map[480][400];
    memset(map, 0, sizeof(map)); // 设置背景为黑色
    double now = 0;
    for (auto i = 0; i < 400; ++i) {
        for (auto j = 0; j < 480; ++j) {
            now++;
            const Node state{0, Vec2d(i - 200, j)};
            if (legalState(state)) {
<<<<<<< HEAD
                double value = getValue(state);
                double k = 255.0 / 600.0, b = 140.0 * k;
                value = k * value + b;
                map[479 - j][i].g = std::min(255, std::max(0,(int)value));
                map[479 - j][i].r = std::min(255, std::max(0, 255-(int)value));
=======
                constexpr auto k = 255.0 / 600.0, b = 140.0 * k;
                const auto value = k * getValue(state) + b;
                map[479 - j][i].g = std::min(255, std::max(0, static_cast<int>(value)));
                map[479 - j][i].r = std::min(255, std::max(0, 255 - static_cast<int>(value)));
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
            }
        }
    }
<<<<<<< HEAD
    generateBmp((BYTE*)map, 400, 480, path);
}
void GameManager::updateEnemyLaserBoxes() noexcept {
	//将激光的判定设为用AABB包起来那么大(gg,先这么写再慢慢改吧)
	for (auto& laser : mLaser) {
		const auto arc = laser.arc - 3.1415926 * 5.0 / 2.0;
		const auto ul = pointRotate(Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y), laser.pos, arc),
			ur = pointRotate(Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y), laser.pos, arc),
			dl = pointRotate(Vec2d(laser.pos.x - laser.size.x / 2.0, laser.pos.y + laser.size.y), laser.pos,
				arc),
			dr = pointRotate(Vec2d(laser.pos.x + laser.size.x / 2.0, laser.pos.y + laser.size.y), laser.pos,
				arc);
		laser.pos = (ul + ur + dl + dr) / 4.0;
		const auto sizeX = std::max(std::max(ul.x, ur.x), std::max(dl.x, dr.x)) - std::min(
			std::min(ul.x, ur.x), std::min(dl.x, dr.x));
		const auto sizeY = std::max(std::max(ul.y, ur.y), std::max(dl.y, dr.y)) - std::min(
			std::min(ul.y, ur.y), std::min(dl.y, dr.y));
		laser.size = Vec2d(sizeX, sizeY);
	}
}
void GameManager::updateBoardInformation(const double ratio) noexcept {
	mConnection->getPlayerData(mPlayer);
	mConnection->getEnemyData(mEnemy);
	mConnection->getEnemyBulletData(mBullet, mPlayer, ratio);
	mConnection->getEnemyLaserData(mLaser);
	mConnection->getPowers(mPowers);
	updateEnemyLaserBoxes();
}
int invincibleTime=0;
void GameManager::update(unsigned long long frameCount,bool enabledMouse) {
	mouseMode = enabledMouse;
	const int maxDepth = 7;
	const int maxInvincibleTime=240;
	updateBoardInformation((double)maxDepth * playerSpeed[0] + 15.0);
	mConnection->getMousePosition(mMousePos);
	mMousePos = fixupPos(mMousePos);
	if (invincibleTime == maxInvincibleTime-60 && mPowers.size() == 0)
	{
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
            if (item.second.value - maxValue > eps) {
                haveNoChoice = false;
                maxValue = item.second.value;
                useShift = item.second.shift;
                moveKeyChoice = keyinfo[item.second.from];
                movement = item.second;
            }
        }
        //扔雷判断
        useBomb = false;
		if (mConnection->GetPlayerStateInformation() == PlayerState::DYING)
		{
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
=======
    generateBmp(map, {400, 480}, path);
}

void GameManager::outputValueMap(const char* path) noexcept {
    updateBoardInformation(1000.0);
    auto fork = *this;
    auto res = std::async(std::launch::async, [fork = std::move(fork), path]() { fork.doValueMapOutput(path); });
}

static int invincibleTime = 0;

void GameManager::updateBoardInformation(const double ratio) noexcept {
    mConnection->getPlayerData(mPlayer);
    mConnection->getEnemyData(mEnemy);
    mConnection->getEnemyBulletData(mBullet, mPlayer, ratio);
    mConnection->getEnemyLaserData(mLaser);
    mConnection->getPowers(mPowers);
    updateEnemyLaserBoxes();
}

void GameManager::pathEnumeration() noexcept {
    //BFS搜索maxDepth步，找到maxDepth步内价值最高的可到达位置。
    valueMap.clear_no_resize();
    bfsQueue.emplace(0, fixupPos(mPlayer.pos));
    while (!bfsQueue.empty()) {
        const auto now = bfsQueue.front();
        const auto nowData = valueMap[compress(now)];
        bfsQueue.pop();
        for (auto i = 0; i < 9; ++i) {
            for (auto j = 0; j <= 1; ++j) {
                const Node nex{now.time + 1, fixupPos(now.pos + direction[i] * playerSpeed[j])};
                if (!legalState(nex))continue;
                if (auto&& [node, success] = valueMap.insert({compress(nex), NodeSave()}); success) {
                    if (now.time == 0)
                        node->second = NodeSave(i, static_cast<bool>(j), getValue(nex));
                    else
                        node->second = NodeSave(nowData.from, nowData.shift, getValue(nex));
                    if (nex.time == maxDepth) bfsQueue.push(nex);
                }
            }
        }
    }
}

bool GameManager::evaluateBombUse() noexcept {
    auto useBomb = false;
    if (invincibleTime <= 0) {
        //1.被子弹打中
        for (const auto& bullet : mBullet) {
            if (hitTestBombChoice(bullet, mPlayer)) {
                useBomb = true;
                invincibleTime = maxInvincibleTime;
                break;
            }
        }
        //2.被体术
        for (const auto& enemy : mEnemy) {
            if (hitTestBombChoice(enemy, mPlayer)) {
                useBomb = true;
                invincibleTime = maxInvincibleTime;
                break;
            }
        }
        //3.被激光打中
        for (const auto& laser : mLaser) {
            if (hitTestBombChoice(laser, mPlayer)) {
                useBomb = true;
                invincibleTime = maxInvincibleTime;
                break;
            }
        }
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    }
    return useBomb;
}

void GameManager::selectBestPath(int& moveKeyChoice, bool& useShift) noexcept {
    auto maxValue = -99999999999.0;
    for (auto&& item : valueMap) {
        if (item.first >> 60 == 0)continue;
        if (item.second.value - maxValue > eps) {
            maxValue = item.second.value;
            useShift = item.second.shift;
            moveKeyChoice = keyinfo[item.second.from];
        }
    }
}

void GameManager::update(const unsigned long long frameCount) noexcept {
    updateBoardInformation(static_cast<double>(maxDepth) * playerSpeed[0] + 15.0);
    if (invincibleTime == maxInvincibleTime - 60 && mPowers.empty()) { invincibleTime = 0; }
    if (invincibleTime > 0)invincibleTime--;

    pathEnumeration();
    //选择最高估价
    auto moveKeyChoice = -1;
    auto useShift = false;
    selectBestPath(moveKeyChoice, useShift);

    const auto useBomb = evaluateBombUse();
    //发送决策
    if (mEnemy.size() <= 1 && mBullet.empty())
        //跳过对话，间隔帧按Z
        mConnection->sendKeyInfo(moveKeyChoice, useShift, frameCount % 2, useBomb);
    else
        //正常进行游戏
        mConnection->sendKeyInfo(moveKeyChoice, useShift, true, useBomb);
}

//判断状态是否合法(某个位置能否到达)
<<<<<<< HEAD
bool GameManager::legalState(Node state) const noexcept {
	if (invincibleTime > 0)return true;
    Object newPlayer = Object(state.pos.x, state.pos.y, mPlayer.size.x, mPlayer.size.y);
=======
bool GameManager::legalState(const Node& state) const noexcept {
    if (invincibleTime > 0)return true;
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    for (auto bullet : mBullet) {
        bullet.pos += bullet.delta * state.time;
        if (hitTest(bullet, mPlayer)) { return false; }
    }
    for (auto enemy : mEnemy) {
        enemy.pos += enemy.delta * state.time;
        if (hitTest(enemy, mPlayer)) { return false; }
    }
	//for (auto laser : mLaser) {
	//	if (hitTest(laser, newPlayer)) { return false; }
	//}
    return true;
}
<<<<<<< HEAD
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
	if (invincibleTime == 0)
	{
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
=======

double GameManager::getThreatValue(const Vec2d& newPos) const noexcept {
    auto avgScore = 0.0, count = 0.0, avgScore2 = 0.0, count2 = 0.0;
    //敌机估价
    for (auto& enemy : mEnemy) {
        constexpr const Vec2d up(0, -1);
        const auto dis = distanceSqr(enemy.pos, newPos);
        auto selfDir = (newPos - enemy.pos).unit();
        //站的位置偏高，容易被弹幕封死。“从敌机指向自机的向量”和“从敌机指向正上方的向量”的夹角越大越安全，减分越少。
        if (enemy.pos.y <= 240) {
            avgScore -= selfDir.dot(up) + 1.0;
            count++;
        }
        //离敌机过进容易被发出的弹幕打死，也可能被体术。因此距离越近减分越多。
        if (dis <= 20000.0) {
            avgScore2 -= 1.0 - dis / 20000.0;
            count2++;
        }
    }
    if (count > 0) avgScore /= count;
    if (count2 > 0) avgScore2 /= count2;
    return 80.0 * avgScore + 80.0 * avgScore2;
}

double GameManager::getAttackValue(const Node& state) const noexcept {
    //子弹估价(和子弹运动方向夹角越大减分越少)
    if (invincibleTime > 0) return 0.0;
    auto avgScore = 0.0, count = 0.0;
    for (auto bullet : mBullet) {
        bullet.pos += bullet.delta * state.time;
        if (distanceSqr(bullet.pos, state.pos) <= 900) {
            count++;
            auto selfDir = (state.pos - bullet.pos).unit();
            const auto bulletDir = bullet.delta.unit();
            avgScore -= selfDir.dot(bulletDir) + 1.0;
        }
    }
    if (count > 0) avgScore /= count;
    return 70.0 * avgScore;
}

double GameManager::getKillValue(const Node& state) const noexcept {
    //击破敌机估价(站在敌机正下方加分)
    auto value = 0.0, minEnemyDis = 400.0;
    if (invincibleTime == 0) {
        for (auto& enemy : mEnemy) {
            auto dis = abs(enemy.pos.x + enemy.delta.x * state.time - state.pos.x);
            minEnemyDis = std::min(minEnemyDis, dis);
        }
        value += 80.0 * (400 - minEnemyDis) / 400;
    }
    return value;
}

double GameManager::getPowerValue(const Node& state) const noexcept {
    //收点估价，离点越近，价值越高   
    auto minPowerDis = 390400.0;
    for (auto& power : mPowers) {
        const auto newPowerPos = power.pos + power.delta * state.time;
        const auto dis = distanceSqr(newPowerPos, state.pos);
        if (dis < minPowerDis) { minPowerDis = dis; }
    }
    return (invincibleTime > 0 ? 480 : 180) * (390400.0 - minPowerDis) / 390400.0;
}

//对状态进行估价
double GameManager::getValue(const Node& state) const noexcept {
    return getPowerValue(state) + 80.0 * getMapValue(state.pos) + getKillValue(state) + getAttackValue(state) +
        getThreatValue(state.pos) - 0.1 * state.time;
}

>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
//修正超出地图的坐标(主要是自机)
Vec2d GameManager::fixupPos(const Vec2d& pos) noexcept {
    auto res = pos;
    if (res.x < ulCorner.x)res.x = ulCorner.x;
    if (res.y < ulCorner.y)res.y = ulCorner.y;
    if (res.x > drCorner.x)res.x = drCorner.x;
    if (res.y > drCorner.y)res.y = drCorner.y;
    return res;
}
//决策时的碰撞检测
bool GameManager::hitTest(const Object& a, const Object& b) noexcept {
    return abs(a.pos.x - b.pos.x) - ((a.size.x + b.size.x) / 2.0) <= eps+0.5 &&
        abs(a.pos.y - b.pos.y) - ((a.size.y + b.size.y) / 2.0) <= eps+0.5;
}
<<<<<<< HEAD
//地图位置估价
double GameManager::getMapValue(Vec2d pos) noexcept {
=======

//地图位置估价(站在地图偏下的位置加分)
double GameManager::getMapValue(const Vec2d& pos) noexcept {
>>>>>>> 9388c79051604a3fcb3dc29a2c558cf186aa0d56
    if (pos.y <= 100)
        return pos.y * 0.9 / 100;
    const auto dis = (abs(390 - pos.y) * (-10.0 / 290.0) + 100.0) / 100.0;
    const auto disx = (200 - abs(0 - pos.x)) / 200.0;
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
