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
            valueMap[startState] = NodeSave(0, 0, getValue(startState));
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
                                valueMap[nex] = NodeSave(i, j, getValue(nex));
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
            if (haveNoChoice) useBomb = true;
            if (mLaser.size() > 0)useBomb = true;
            if (mEnemy.size() <= 1 && mBullet.size() == 0)
                mConnection->sendKeyInfo(moveKeyChoice, useShift, frameCount % 2, useBomb);
            else
                mConnection->sendKeyInfo(moveKeyChoice, useShift, true, useBomb);
            break;
        }
        default:
            break;
    }
}

bool operator<(const Node &lhs, const Node &rhs) {
    if (lhs.time < rhs.time)return true;
    if (lhs.time > rhs.time)return false;
    if ((rhs.pos.x - lhs.pos.x) > eps)return true;
    if ((lhs.pos.x - rhs.pos.x) > eps)return false;
    if ((rhs.pos.y - lhs.pos.y) > eps)return true;
    return false;
}
