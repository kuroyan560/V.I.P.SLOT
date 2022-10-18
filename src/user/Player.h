#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
#include"Timer.h"
#include<vector>
#include"CollisionCallBack.h"
#include"Transform.h"
#include"PlayerAttackHitEffect.h"
#include"ColliderParentObject.h"
class ModelObject;
class LightManager;
class Camera;
class SlotMachine;
class TimeScale;
class Collider;
class CollisionManager;
class GameCamera;

class Player : public ColliderParentObject
{
	//モデルオブジェクト
	std::shared_ptr<ModelObject>m_modelObj;

	//移動
	Vec3<float>m_move = { 0,0,0 };

	//落下速度
	float m_fallSpeed;

	//ジャンプ権
	bool m_canJump;
	//接地フラグ
	bool m_isOnGround;
	//ジャンプSE
	int m_jumpSE;

	//所持金
	CoinVault m_coinVault;

	//コライダー
	std::shared_ptr<Collider>m_bodyCollider;
	//std::shared_ptr<Collider>m_footCollider;

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

		//ヒットストップSE
		int m_hitStopSE;
		//ダメージSE
		int m_damageSE;

		void OnCollisionEnter(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const CollisionManager& arg_collisionMgr)override {};

		void OnCollisionTrigger(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const CollisionManager& arg_collisionMgr)override;

	public:
		DamagedCallBack(Player* arg_player, int arg_hitStopSE, int arg_damageSE)
			:m_parent(arg_player), m_hitStopSE(arg_hitStopSE), m_damageSE(arg_damageSE) {}
		void Init(std::weak_ptr<GameCamera>arg_cam)
		{
			m_cam = arg_cam;
			m_invincibleTimer.Reset(0);
			m_hitStopTimer.Reset(0);
			m_isDraw = true;
		}
		void Update(TimeScale& arg_timeScale);

		const bool& GetIsDraw()const { return m_isDraw; }
	};
	std::shared_ptr<DamagedCallBack>m_damegedCallBack;

	//ブロックに触れた際のジャンプ権回復コールバック
	class CallBackWithBlock : public CollisionCallBack
	{
		Player* m_parent;

		void OnCollisionEnter(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const CollisionManager& arg_collisionMgr)override {}

		void OnCollisionTrigger(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const CollisionManager& arg_collisionMgr)override;
	public:
		CallBackWithBlock(Player* arg_player) :m_parent(arg_player) {}
	};
	std::shared_ptr<CallBackWithBlock>m_callBackWithBlock;

	//ジャンプ
	void Jump(Vec3<float>* arg_rockOnPos = nullptr);

	//操作がキーボードかコントローラーか
	enum struct INPUT_CONFIG { KEY_BOARD, CONTROLLER };
	INPUT_CONFIG m_inputConfig = INPUT_CONFIG::CONTROLLER;
	
public:
	Player(std::weak_ptr<CollisionManager>arg_collisionMgr);
	//初期化
	void Init(std::weak_ptr<GameCamera>arg_cam);
	//更新
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);
	//エフェクト描画
	void EffectDraw(std::weak_ptr<Camera>arg_cam);

	//プレイヤーのモデル中央に合わせた座標ゲッタ
	Vec3<float>GetCenterPos()const;
};