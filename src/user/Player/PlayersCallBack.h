#pragma once
#include"CollisionCallBack.h"
#include<memory>
#include"Timer.h"
#include"PlayersCounterAttackHitEffect.h"
class CoinVault;
class TexHitEffect;
class ObjectManager;
class GameCamera;
class Player;
class TimeScale;

//通常攻撃
class PlayersNormalAttack : public CollisionCallBack
{
	//攻撃力のポインタ
	int* m_offensive = nullptr;
	//プレイヤーの所持金のポインタ
	CoinVault* m_playersVault = nullptr;
	//ヒットエフェクト
	std::weak_ptr<TexHitEffect>m_hitEffect;
	//SE
	int m_hitSE;
	int m_killSE;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {};

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;
public:
	PlayersNormalAttack(
		int* arg_offensive,
		CoinVault* arg_playersVault,
		std::weak_ptr<TexHitEffect>arg_hitEffect,
		int arg_hitSE,
		int arg_killSE)
		:m_offensive(arg_offensive), m_playersVault(arg_playersVault), m_hitEffect(arg_hitEffect), m_hitSE(arg_hitSE), m_killSE(arg_killSE) {}
};

//パリィ攻撃
class PlayersParryAttack : public CollisionCallBack
{
	//攻撃力のポインタ
	int* m_offensive = nullptr;
	//オブジェクトマネージャポインタ（パリィ弾発射用）
	std::weak_ptr<ObjectManager>m_objMgr;
	//コリジョンマネージャポインタ（パリィ弾発射用）
	std::weak_ptr<CollisionManager>m_colMgr;
	//SE
	int m_parrySE;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {};

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;

public:
	PlayersParryAttack(
		int* arg_offensive,
		std::weak_ptr<ObjectManager>arg_objMgr,
		std::weak_ptr<CollisionManager>arg_colMgr,
		int arg_parrySE)
		:m_offensive(arg_offensive), m_objMgr(arg_objMgr), m_colMgr(arg_colMgr), m_parrySE(arg_parrySE) {}
};

//相手の攻撃をパリィによって自身のものに、それを利用した攻撃
class PlayersCounterAttack : public CollisionCallBack
{
	//攻撃力のポインタ
	int* m_offensive = nullptr;
	//プレイヤーの所持金のポインタ
	CoinVault* m_playersVault = nullptr;
	//ヒットエフェクト
	PlayersCounterAttackHitEffect* m_hitEffect = nullptr;
	//SE
	int m_hitSE;
	int m_killSE;

	//その攻撃が当たった敵の数
	int m_hitCount = 0;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {};

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;
public:
	PlayersCounterAttack(
		int* arg_offensive,
		CoinVault* arg_playersVault,
		PlayersCounterAttackHitEffect* arg_hitEffect,
		int arg_hitSE,
		int arg_killSE)
		:m_offensive(arg_offensive), m_playersVault(arg_playersVault), m_hitEffect(arg_hitEffect), m_hitSE(arg_hitSE), m_killSE(arg_killSE) {}
};

//被ダメージ
class DamagedCallBack : public CollisionCallBack
{
	Player* m_parent;
	std::weak_ptr<GameCamera>m_cam;

	//ヒットストップタイマー
	Timer m_hitStopTimer;

	//無敵時間
	Timer m_invincibleTimer;

	//無敵時間中点滅
	Timer m_flashTimer;
	bool m_isDraw;

	//ヒットストップSE
	int m_hitStopSE;
	//ダメージSE
	int m_damageSE;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {};

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;

public:
	DamagedCallBack(Player* arg_player, std::weak_ptr<GameCamera>arg_cam, int arg_hitStopSE, int arg_damageSE)
		:m_parent(arg_player), m_cam(arg_cam), m_hitStopSE(arg_hitStopSE), m_damageSE(arg_damageSE) {}
	void Init()
	{
		m_invincibleTimer.Reset(0);
		m_hitStopTimer.Reset(0);
		m_isDraw = true;
	}
	void Update(TimeScale& arg_timeScale);

	const bool& GetIsDraw()const { return m_isDraw; }

	//デバッグ用に外部から呼び出せるように
	void Execute();
};