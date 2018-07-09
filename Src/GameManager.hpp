#pragma once
#include<vector>
#include<memory>
#include<map>
#include "Vec2.hpp"
#include "Object.hpp"
#include<queue>
const double s2d2 = sqrt(2.0) / 2.0;
const double dx[9] = { 0,1.0,s2d2,0,-s2d2 ,-1.0,-s2d2 ,0,s2d2 };
const double dy[9] = { 0,0,-s2d2,-1.0,-s2d2 ,0,s2d2 ,1.0,s2d2 };
const double playerSpeed[2] = {9.0,4.0};
const Vec2d ulCorner = Vec2d(-200, 0);
const Vec2d drCorner = Vec2d(200, 480);
const double eps = 1e-7;
//从高位到低位分别为上下左右
const int keyinfo[9] = { 0x0,0x1,0x9,0x8,0xa,0x2,0x6,0x4,0x5};
enum class GameState
{
	NORMAL,
	COLLECT,
	MOVE
};
struct Node
{
	int8_t time;
	Vec2d pos;
	Node() { time = -1, pos = Vec2d(-1, -1); }
	Node(int8_t time_, Vec2d pos_)
	{
		time = time_; pos = pos_;
	}
};
bool operator <(const Node& lhs,const Node& rhs)
{
	if (lhs.time < rhs.time)return true;
	if (lhs.time > rhs.time)return false;
	if ((rhs.pos.x - lhs.pos.x) > eps)return true;
	if ((lhs.pos.x - rhs.pos.x) > eps)return false;
	if ((rhs.pos.y - lhs.pos.y) > eps)return true;
	return false;
}
struct NodeSave
{
	bool shift;
	int from;
	double value;
	NodeSave() {}
	NodeSave(int from_,bool shift_,double value_)
	{
		from = from_;
		shift = shift_;
		value = value_;
	}
};
class GameManager
{
public:
	GameManager(std::shared_ptr<HANDLE> hprocess)
	{
		mHprocess = hprocess;
		mState = GameState::NORMAL;
	}
	void update(unsigned long long frameCount)
	{
		const int maxDepth = 4;
		this->GetPlayerData(mPlayer);
		this->GetEnemyData(mEnemy);
		this->GetEnemyBulletData(mBullet, (double)maxDepth*playerSpeed[0]+15.0);
		this->GetEnemyLaserData(mLaser);
		this->GetPowers(mPowers);
		
		//确定当前状态
		//超过5个P点就尝试上线收点
		//if (mPowers.size() > 5)
		//{
		//	mState = GameState::COLLECT;
		//}else 
		mState = GameState::NORMAL;
		switch(mState)
		{
		case GameState::NORMAL:
		{
			//bool ok = false;
			//double value[9][2];
			valueMap.erase(valueMap.begin(), valueMap.end());	
			Node startState = Node(0, fixupPos(mPlayer.pos));
			valueMap[startState] = NodeSave(0, 0, getValue(startState));
			bfsQueue.push(startState);
			while (!bfsQueue.empty())
			{
				Node now = bfsQueue.front();
				NodeSave nowData = valueMap[now];
				bfsQueue.pop();
				if (now.time >= maxDepth)continue;
				for (int i = 0; i < 9; ++i)
				{
					for (int j = 0; j <= 1; ++j)
					{
						Node nex = Node(now.time + 1, fixupPos(Vec2d(now.pos.x + dx[i] * playerSpeed[j], now.pos.y + dy[i] * playerSpeed[j])));
						if (valueMap.find(nex) == valueMap.end())
						{
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
			for(auto&& item:valueMap)
			{ 
				//printf("%d\n", item.second.value);
				if (item.first.time == 0)continue;
				//printf("%.3lf %.3lf %.3lf\n",item.first.pos.x,item.first.pos.y,item.second.value);
				if (item.second.value - maxValue>eps)
				{
					haveNoChoice = false;
					maxValue = item.second.value;
					useShift = item.second.shift;
					moveKeyChoice = keyinfo[item.second.from];
				}
			}
			//std::cout<<"value:" << maxValue << std::endl;
			if (haveNoChoice)useBomb = true;
			if (mLaser.size() > 0)useBomb = true;
			if(mEnemy.size()<=1&&mBullet.size()==0)	
				KeyboardManager::sendKeyInfo(moveKeyChoice, useShift, frameCount%2, useBomb);
			else
				KeyboardManager::sendKeyInfo(moveKeyChoice, useShift, true, useBomb);
			break;
		}
		default:
			break;
		}
	}
private:
	std::map<Node, NodeSave>valueMap;
	std::queue<Node>bfsQueue;
	GameState mState;
	Player mPlayer;
	std::vector<Object> mEnemy;
	std::vector<Object> mBullet;
	std::vector<Laser> mLaser;
	std::vector<Object> mPowers;
	std::shared_ptr<HANDLE> mHprocess;
	bool legalState(Node state)
	{
		Object newPlayer = Object(state.pos.x, state.pos.y, mPlayer.size.x, mPlayer.size.y);
		for (int i = 0; i < mBullet.size(); ++i)
		{
			Object bullet = mBullet[i];
			bullet.pos += bullet.delta*state.time;
			if (hittest(bullet, newPlayer))
			{
				return false;
			}
		}
		for (int i = 0; i < mEnemy.size(); ++i)
		{
			Object enemy = mEnemy[i];
			enemy.pos += enemy.delta*state.time;
			if (hittest(enemy, newPlayer))
			{
				return false;
			}
		}
		return true;
	}
	//估价效率过低，待修改
	double getValue(Node state)
	{
		double value=0.0;
		//初次估价
		double minEnemyDis = 400.0;
		Vec2d newPos = state.pos;
		Object newPlayer = Object(newPos.x, newPos.y, mPlayer.size.x, mPlayer.size.y);
		minEnemyDis = 400.0;
		//P点距离估价
		double minPowerDis = 390400.0;
		double y = -1;
		for (int i = 0; i < mPowers.size(); ++i)
		{
			Vec2d newPowerPos = mPowers[i].pos + mPowers[i].delta*state.time;
			double dis = getSquareDis(newPowerPos, newPos);
			if (dis < minPowerDis)
			{
				minPowerDis = dis;
				y = newPowerPos.y;
			}
		}
		if (y >= 250)
			value += 160 * (390400.0 - minPowerDis) / 390400.0;
		else
			value += 80 * (390400.0 - minPowerDis) / 390400.0;
		//地图位置估价
		value += 10.0*getMapValue(newPos);
		//击破敌机估价
		for (int i = 0; i < mEnemy.size(); ++i)
		{
			double dis = abs(mEnemy[i].pos.x+mEnemy[i].delta.x*state.time - newPos.x);
			minEnemyDis = min(minEnemyDis, dis);
		}
		value += 80.0*(400.0 - minEnemyDis) / 400.0;
		//value += rand() % 10/100.00;
		value -=0.1*state.time;
		return value;
	}
	Vec2d fixupPos(const Vec2d& pos)
	{
		Vec2d res=pos;
		if (res.x < ulCorner.x)res.x = ulCorner.x;
		if (res.y < ulCorner.y)res.y = ulCorner.y;
		if (res.x > drCorner.x)res.x = drCorner.x;
		if (res.y > drCorner.y)res.y = drCorner.y;
		return res;
	}
	bool hittest(Object& a, Object& b)
	{
		if (abs(a.pos.x - b.pos.x) <= ((a.size.x + b.size.x) / 2.0)+5 && abs(a.pos.y - b.pos.y) <= ((a.size.y + b.size.y) / 2.0)+5)return true;
		return false;
	}
	//地图位置估价
	double getMapValue(Vec2d pos)
	{
		double dis = abs(390 - pos.y);
		double disx = abs(0 - pos.x)/10;
		return ((410.0) -dis- disx) / (410.0);
		/*
		//最中心一块(-100,240)~(100,440)
		if (pos.x >= -100 && pos.x <= 100 && pos.y >= 240 && pos.y <= 440)return 1.00;
		//中间层(-180,100)~(180,480)
		if (pos.x >= -180 && pos.x <= 180 && pos.y >= 100 && pos.y <= 480)return 0.80;
		//外层
		*/
		//return 0.60;
	}
	void GetPowers(std::vector<Object>& powers)
	{
		DWORD nbr;
		powers.clear();
		int base;
		ReadProcessMemory(*mHprocess, (LPCVOID)0x00477818, &base, 4, &nbr);
		if (base == NULL)
		{
			return;
		}
		int esi = base + 0x14;
		int ebp = esi + 0x3B0;

		for (int i = 0; i < 2000; i++)
		{
			int eax;
			ReadProcessMemory(*mHprocess, (LPCVOID)(ebp + 0x2C), &eax, 4, &nbr);
			if (eax != 0)
			{
				float x, y;
				ReadProcessMemory(*mHprocess, (LPCVOID)(ebp - 0x4), &x, 4, &nbr);
				ReadProcessMemory(*mHprocess, (LPCVOID)(ebp), &y, 4, &nbr);
				powers.push_back(Object(x, y, 6, 6));
			}
			ebp += 0x3F0;
		}
	}
	void GetEnemyData(std::vector<Object>& enemy)
	{
		DWORD nbr;
		int base, obj_base, obj_addr, obj_next;
		enemy.clear();
		ReadProcessMemory(*mHprocess, (LPCVOID)0x00477704, &base, 4, &nbr);
		if (base == NULL)
		{
			return;
		}
		ReadProcessMemory(*mHprocess, (LPCVOID)(base + 0x58), &obj_base, 4, &nbr);
		if (obj_base != NULL)
		{
			while (true)
			{
				ReadProcessMemory(*mHprocess, (LPCVOID)obj_base, &obj_addr, 4, &nbr);
				ReadProcessMemory(*mHprocess, (LPCVOID)(obj_base + 4), &obj_next, 4, &nbr);
				obj_addr += 0x103C;
				unsigned int t;
				ReadProcessMemory(*mHprocess, (LPCVOID)(obj_addr + 0x1444), &t, 4, &nbr);
				if (!(t & 0x40)) {
					ReadProcessMemory(*mHprocess, (LPCVOID)(obj_addr + 0x1444), &t, 4, &nbr);
					if (!(t & 0x12)) {
						float x, y, w, h;
						ReadProcessMemory(*mHprocess, (LPCVOID)(obj_addr + 0x2C), &x, 4, &nbr);
						ReadProcessMemory(*mHprocess, (LPCVOID)(obj_addr + 0x30), &y, 4, &nbr);
						ReadProcessMemory(*mHprocess, (LPCVOID)(obj_addr + 0xB8), &w, 4, &nbr);
						ReadProcessMemory(*mHprocess, (LPCVOID)(obj_addr + 0xBC), &h, 4, &nbr);
						
						enemy.push_back(Object(x, y, w, h));
					}
				}
				if (obj_next == 0) {
					break;
				}
				obj_base = obj_next;
			}
		}
	}

	void GetEnemyBulletData(std::vector<Object>& bullet,double maxRange)
	{
		bullet.clear();
		int base;
		DWORD nbr;
		ReadProcessMemory(*mHprocess, (LPCVOID)0x004776F0, &base, 4, &nbr);
		if (base == NULL)
		{
			return;
		}
		int ebx = base + 0x60;
		for (int i = 0; i < 2000; i++)
		{
			int edi = ebx + 0x400;
			int bp;
			ReadProcessMemory(*mHprocess, (LPCVOID)(edi + 0x46), &bp, 4, &nbr);
			bp = bp & 0x0000FFFF;
			if (bp)
			{
				int eax;
				ReadProcessMemory(*mHprocess, (LPCVOID)(0x477810), &eax, 4, &nbr);
				if (eax != NULL)
				{
					ReadProcessMemory(*mHprocess, (LPCVOID)(eax + 0x58), &eax, 4, &nbr);
					if (!(eax & 0x00000400)) {
						float x, y, w, h, dx, dy;
						ReadProcessMemory(*mHprocess, (LPCVOID)(ebx + 0x3C0), &dx, 4, &nbr);
						ReadProcessMemory(*mHprocess, (LPCVOID)(ebx + 0x3C4), &dy, 4, &nbr);
						ReadProcessMemory(*mHprocess, (LPCVOID)(ebx + 0x3B4), &x, 4, &nbr);
						ReadProcessMemory(*mHprocess, (LPCVOID)(ebx + 0x3B8), &y, 4, &nbr);
						ReadProcessMemory(*mHprocess, (LPCVOID)(ebx + 0x3F0), &w, 4, &nbr);
						ReadProcessMemory(*mHprocess, (LPCVOID)(ebx + 0x3F4), &h, 4, &nbr);
						//为了效率，只考虑可能会碰到的子弹
						if(getSquareDis(Vec2d(x,y),mPlayer.pos)<=maxRange*maxRange)
							bullet.push_back(Object(x, y, w, h, dx / 2.0f, dy / 2.0f));
					}
				}
			}
			ebx += 0x7F0;
		}
	}
	void GetPlayerData(Player& self)
	{
		float x, y;
		int obj_base;
		DWORD nbr;
		ReadProcessMemory(*mHprocess, (LPCVOID)0x00477834, &obj_base, 4, &nbr);
		if (obj_base == NULL)
		{
			return;
		}
		ReadProcessMemory(*mHprocess, (LPCVOID)(obj_base + 0x3C0), &x, 4, &nbr);
		ReadProcessMemory(*mHprocess, (LPCVOID)(obj_base + 0x3C4), &y, 4, &nbr);
		self.pos.x = x;
		self.pos.y = y;
	}
	void GetEnemyLaserData(std::vector<Laser>& laser)
	{
		laser.clear();
		int base;
		DWORD nbr;
		ReadProcessMemory(*mHprocess, (LPCVOID)0x0047781C, &base, 4, &nbr);
		if (base == NULL)
		{
			return;
		}
		int esi, ebx;
		ReadProcessMemory(*mHprocess, (LPCVOID)(base + 0x18), &esi, 4, &nbr);
		if (esi != NULL)
		{
			while (true)
			{
				ReadProcessMemory(*mHprocess, (LPCVOID)(esi + 0x8), &ebx, 4, &nbr);
				float x, y, h, w, arc;
				ReadProcessMemory(*mHprocess, (LPCVOID)(esi + 0x24), &x, 4, &nbr);
				ReadProcessMemory(*mHprocess, (LPCVOID)(esi + 0x28), &y, 4, &nbr);
				ReadProcessMemory(*mHprocess, (LPCVOID)(esi + 0x3C), &arc, 4, &nbr);
				ReadProcessMemory(*mHprocess, (LPCVOID)(esi + 0x40), &h, 4, &nbr);
				ReadProcessMemory(*mHprocess, (LPCVOID)(esi + 0x44), &w, 4, &nbr);
				laser.push_back(Laser(x, y, w / 2.0f, h, arc));
				if (ebx == NULL)
				{
					break;
				}
				esi = ebx;
			}
		}
	}
	double getDis(Vec2d point1,Vec2d point2)
	{
		return sqrt((point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y));
	}
	double getSquareDis(Vec2d point1, Vec2d point2)
	{
		return (point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y);
	}
};