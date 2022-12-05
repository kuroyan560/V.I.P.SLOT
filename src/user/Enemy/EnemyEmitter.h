#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
#include"Debugger.h"
#include"ConstParameters.h"
#include<map>

class ObjectManager;
class CollisionManager;
class TimeScale;
class EnemyEmitter : public Debugger
{
	using TYPE = ConstParameter::Enemy::ENEMY_TYPE;
	//出現率（％）
	std::array<int, static_cast<int>(TYPE::NUM)>m_appearRate;
	//出現させる時間間隔
	int m_appearSpan = 180;
	//一度に出現させる数
	int m_appearNum = 3;

	//時間計測
	Timer m_timer;

	enum { IMGUI_RANDOM_APPEAR, IMGUI_CUSTOM_PARAM }m_imguiDebugMode = IMGUI_CUSTOM_PARAM;

	/*--- 各種敵のパラメータ ---*/
	std::map<TYPE, std::map<std::string, float>>m_customParams;

	void EmitEnemy(std::weak_ptr<ObjectManager>& arg_objMgr, std::weak_ptr<CollisionManager>& arg_colMgr, TYPE arg_type);
	void EmitEnemys(std::weak_ptr<ObjectManager>& arg_objMgr, std::weak_ptr<CollisionManager>& arg_colMgr);
	void UpdateAppearRate(int arg_fixRateTypeIdx);
	void OnImguiItems()override;

public:
	EnemyEmitter();
	void Init(std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<CollisionManager> arg_colMgr);

	//テスト用に適当に敵を出現させる
	void TestRandEmit(const TimeScale& arg_timeScale, std::weak_ptr<ObjectManager>arg_objMgr, std::weak_ptr<CollisionManager>arg_colMgr);
};