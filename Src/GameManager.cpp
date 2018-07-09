#include "GameManager.hpp"

void GameManager::update(unsigned long long frameCount) {
    const int maxDepth = 4;
    mConnection->GetPlayerData(mPlayer);
    mConnection->GetEnemyData(mEnemy);
    mConnection->GetEnemyBulletData(mBullet, mPlayer, (double) maxDepth * playerSpeed[0] + 15.0);
    mConnection->GetEnemyLaserData(mLaser);
    mConnection->GetPowers(mPowers);

    //确定当前状态
    //超过5个P点就尝试上线收点
    //if (mPowers.size() > 5)
    //{
    //	mState = GameState::COLLECT;
    //}else
    mState = GameState::NORMAL;
    switch (mState) {
        case GameState::NORMAL: {
            //bool ok = false;
            //double value[9][2];
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
            //选择最高估价以及信息输出
            //printf("%d\n", valueMap.size());
            bool haveNoChoice = true;
            double maxValue = -99999999999.0;
            int moveKeyChoice = -1;
            bool useShift = false;
            bool useBomb = false;
            for (auto &&item:valueMap) {
                //printf("%d\n", item.second.value);
                if (item.first.time == 0)continue;
                //printf("%.3lf %.3lf %.3lf\n",item.first.pos.x,item.first.pos.y,item.second.value);
                if (item.second.value - maxValue > eps) {
                    haveNoChoice = false;
                    maxValue = item.second.value;
                    useShift = item.second.shift;
                    moveKeyChoice = keyinfo[item.second.from];
                }
            }
            //std::cout<<"value:" << maxValue << std::endl;
            if (haveNoChoice)useBomb = true;
            if (!mLaser.empty())useBomb = true;
            if (mEnemy.size() <= 1 && mBullet.empty())
                mConnection->sendKeyInfo(moveKeyChoice, useShift, frameCount % 2, useBomb);
            else
                mConnection->sendKeyInfo(moveKeyChoice, useShift, true, useBomb);
            break;
        }
        default:
            break;
    }
}

bool GameManager::legalState(Node state) {
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

double GameManager::getValue(Node state) {
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
    //if (y >= 250)
    //	value += 200 * (390400.0 - minPowerDis) / 390400.0;
    //else
    value += 100 * (390400.0 - minPowerDis) / 390400.0;
    //地图位置估价
    value += 15.0 * getMapValue(newPos);
    //击破敌机估价
    for (auto &enemy : mEnemy) {
        double dis = abs(enemy.pos.x + enemy.delta.x * state.time - newPos.x);
        minEnemyDis = std::min(minEnemyDis, dis);
    }
    value += 80.0 * (400.0 - minEnemyDis) / 400.0;
    //子弹估价
    double avgScore = 0;
    double count = 0;
    for (auto bullet : mBullet) {
        bullet.pos += bullet.delta * state.time;
        if (getDis(bullet.pos, newPos) <= 30) {
            count++;
            Vec2d dirVec = (newPos - bullet.pos).unit();
            Vec2d bulletDir = bullet.delta.unit();
            //方向d的价值与方向d和子弹运动方向的夹角有关，夹角越大，价值越高
            double dirvalue = dirVec.dot(bulletDir);
            avgScore -= dirvalue + 1.0;
        }
    }
    if (count > 0) {
        avgScore /= count;
        value += 80.0 * avgScore;
    }
    //value += rand() % 10/100.00;
    value -= 0.1 * state.time;
    return value;
}

Vec2d GameManager::fixupPos(const Vec2d &pos) {
    Vec2d res = pos;
    if (res.x < ulCorner.x)res.x = ulCorner.x;
    if (res.y < ulCorner.y)res.y = ulCorner.y;
    if (res.x > drCorner.x)res.x = drCorner.x;
    if (res.y > drCorner.y)res.y = drCorner.y;
    return res;
}

bool GameManager::hittest(Object &a, Object &b) {
    return abs(a.pos.x - b.pos.x) <= ((a.size.x + b.size.x) / 2.0) + 5 &&
    abs(a.pos.y - b.pos.y) <= ((a.size.y + b.size.y) / 2.0) + 5;
}

double GameManager::getMapValue(Vec2d pos) {
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

bool operator<(const Node &lhs, const Node &rhs) {
    if (lhs.time < rhs.time)return true;
    if (lhs.time > rhs.time)return false;
    if ((rhs.pos.x - lhs.pos.x) > eps)return true;
    if ((lhs.pos.x - rhs.pos.x) > eps)return false;
    if ((rhs.pos.y - lhs.pos.y) > eps)return true;
    return false;
}
