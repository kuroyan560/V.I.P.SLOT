#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
#include"Timer.h"
#include<vector>
#include"CollisionCallBack.h"
#include"Transform.h"
class ModelObject;
class LightManager;
class Camera;
class SlotMachine;
class TimeScale;
class Collider;
class CollisionManager;
class GameCamera;

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
	//ジャンプSE
	int m_jumpSE;

	//所持金
	CoinVault m_coinVault;

	//BETのスパン計測用タイマー
	Timer m_betTimer;
	//連続BETの計測用タイマー
	Timer m_consecutiveBetTimer;
	//BETのSE
	int m_betSE;

	//コライダー
	std::shared_ptr<Collider>m_bodyCollider;
	std::shared_ptr<Collider>m_footCollider;

	//HP
	int m_hp;

	/*--- コールバック関数 ---*/
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

		//敵踏みつけ可能になるまでの時間
		Timer m_canStepTimer;

		//ヒットストップSE
		int m_hitStopSE;
		//ダメージSE
		int m_damageSE;

		void OnCollision(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const unsigned char& arg_otherAttribute,
			const CollisionManager& arg_collisionMgr)override;

	public:
		DamagedCallBack(Player* arg_player, int arg_hitStopSE, int arg_damageSE)
			:m_parent(arg_player), m_hitStopSE(arg_hitStopSE), m_damageSE(arg_damageSE) {}
		void Init(std::weak_ptr<GameCamera>arg_cam)
		{
			m_cam = arg_cam;
			m_invincibleTimer.Reset(0);
			m_hitStopTimer.Reset(0);
			m_canStepTimer.Reset(0);
			m_isDraw = true;
		}
		void Update(TimeScale& arg_timeScale);

		const bool& GetIsDraw()const { return m_isDraw; }
		bool IsCanStep()const { return m_canStepTimer.IsTimeUp(); }
	};
	std::shared_ptr<DamagedCallBack>m_damegedCallBack;

	//敵への攻撃
	class AttackCallBack : public CollisionCallBack
	{
		Player* m_parent;
		//踏みつけSE
		int m_stepEnemySE;
		//無敵時間
		Timer m_invincibleTimer;

		void OnCollision(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const unsigned char& arg_otherAttribute,
			const CollisionManager& arg_collisionMgr)override;

	public:
		AttackCallBack(Player* arg_player, int arg_stepEnemySE)
			:m_parent(arg_player), m_stepEnemySE(arg_stepEnemySE) {}
		void Init()
		{
			m_invincibleTimer.Reset(0);
		}
		void Update(float arg_timeScale)
		{
			m_invincibleTimer.UpdateTimer(arg_timeScale);
		}
		bool GetIsInvincible()const { return !m_invincibleTimer.IsTimeUp(); }
	};
	std::shared_ptr<AttackCallBack>m_attackCallBack;

	void Jump();
	
public:
	Player(std::weak_ptr<CollisionManager>arg_collisionMgr);
	//初期化
	void Init(std::weak_ptr<GameCamera>arg_cam);
	//更新
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//コインGET（＋n表示）
	void GetCoin(int arg_coinNum);
	void GetCoin(CoinVault& arg_coinVault)
	{
		arg_coinVault.Pass(m_coinVault);
		GetCoin(0);
	}
	void GetCoin(CoinVault& arg_coinVault, int arg_coinNum)
	{
		arg_coinVault.Pass(m_coinVault, arg_coinNum);
		GetCoin(0);
	}

	//コインを渡す
	void PassCoin(CoinVault& arg_coinVault)
	{
		m_coinVault.Pass(arg_coinVault);
	}
	void PassCoin(CoinVault& arg_coinVault, int arg_coinNum)
	{
		m_coinVault.Pass(arg_coinVault, arg_coinNum);
	}

	//トランスフォームゲッタ
	const Transform& GetTransform()const;
};