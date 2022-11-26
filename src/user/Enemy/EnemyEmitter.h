#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
class ObjectManager;
class CollisionManager;
class TimeScale;
class EnemyEmitter
{
	enum TYPE { SLIDE_MOVE, SLIME_BATTERY, NUM };

	//出現率（％）
	std::array<int, TYPE::NUM>m_appearRate = { 50,50 };
	//出現させる時間間隔
	int m_appearSpan = 300;
	//一度に出現させる数
	int m_appearNum = 1;

	//時間計測
	Timer m_timer;

	void EmitEnemy(std::weak_ptr<ObjectManager>& arg_objMgr, std::weak_ptr<CollisionManager>& arg_colMgr, int arg_type);
	void EmitEnemys(std::weak_ptr<ObjectManager>& arg_objMgr, std::weak_ptr<CollisionManager>& arg_colMgr);
	void UpdateAppearRate(int arg_fixRateTypeIdx);

public:
	void Init(std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<CollisionManager> arg_colMgr);

	//テスト用に適当に敵を出現させる
	void TestRandEmit(const TimeScale& arg_timeScale, std::weak_ptr<ObjectManager>arg_objMgr, std::weak_ptr<CollisionManager>arg_colMgr);

	//テスト出現のパラメータ調整
	void ImguiDebug();
};