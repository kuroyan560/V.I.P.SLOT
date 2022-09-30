#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
#include"Timer.h"
#include<vector>
#include"CollisionCallBack.h"
class ModelObject;
class LightManager;
class Camera;
class SlotMachine;
class TimeScale;
class Collider;
class CollisionManager;

class Player
{
	//モデルオブジェクト
	std::shared_ptr<ModelObject>m_modelObj;

	//移動
	Vec3<float>m_move = { 0,0,0 };

	//落下速度
	float m_fallSpeed;

	//接地フラグ
	bool m_isOnGround;

	//所持金
	CoinVault m_coinVault;

	//BETのスパン計測用タイマー
	Timer m_betTimer;
	//連続BETの計測用タイマー
	Timer m_consecutiveBetTimer;
	//BETのSE
	int m_betSE;

	//コライダー
	std::vector<std::shared_ptr<Collider>>m_colliders;

	//HP
	int m_hp;

	/*--- コールバック関数 ---*/
	//被ダメージ
	class DamagedCallBack : public CollisionCallBack
	{
		Player* m_parent;

		//ヒットストップタイマー
		Timer m_hitStopTimer;

		//無敵時間
		Timer m_invincibleTimer;

		void OnCollision(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const unsigned char& arg_otherAttribute,
			const CollisionManager& arg_collisionMgr)override;

	public:
		DamagedCallBack(Player* arg_player) :m_parent(arg_player) {}
		void Init()
		{
			m_invincibleTimer.Reset(0);
			m_hitStopTimer.Reset(0);
		}
		void Update(TimeScale& arg_timeScale);
	};
	std::shared_ptr<DamagedCallBack>m_damegedCallBack;

	//敵への攻撃
	class AttackCallBack : public CollisionCallBack
	{
		Player* m_parent;

		void OnCollision(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const unsigned char& arg_otherAttribute,
			const CollisionManager& arg_collisionMgr)override;

	public:
		AttackCallBack(Player* arg_player) :m_parent(arg_player) {}
	};
	std::shared_ptr<AttackCallBack>m_attackCallBack;

	void Jump();
	
public:
	Player(std::weak_ptr<CollisionManager>arg_collisionMgr);
	//初期化
	void Init();
	//更新
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	CoinVault& GetVault() { return m_coinVault; }
};