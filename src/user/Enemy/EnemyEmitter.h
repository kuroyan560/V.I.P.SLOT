#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
#include"Debugger.h"
#include"ConstParameters.h"
#include<map>
#include<forward_list>

class ObjectManager;
class CollisionManager;
class TimeScale;
class GameObject;
class EnemyEmitter : public Debugger
{
	std::weak_ptr<ObjectManager>m_referObjMgr;
	std::weak_ptr<CollisionManager>m_referCollisionMgr;

	//敵のオブジェクトポインタ配列
	std::forward_list<std::weak_ptr<GameObject>>m_enemyObjList;

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

	void RandEmitEnemy(TYPE arg_type);
	void RandEmitEnemys();
	void UpdateAppearRate(int arg_fixRateTypeIdx);
	void OnImguiItems()override;

public:
	EnemyEmitter();
	void Awake(std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<CollisionManager> arg_colMgr)
	{
		m_referObjMgr = arg_objMgr;
		m_referCollisionMgr = arg_colMgr;
	}
	void Init();

	//敵の出現
	void EmitEnemy(TYPE arg_type, Vec3<float>arg_initPos);

	//テスト用に適当に敵を出現させる
	void TestRandEmit(const TimeScale& arg_timeScale);

	/// <summary>
	/// 全ての敵を一掃
	/// </summary>
	/// <returns>落としたコイン合計</returns>
	int KillAllEnemys();
};