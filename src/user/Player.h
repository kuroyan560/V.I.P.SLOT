#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
#include"Timer.h"
#include<vector>
#include"CollisionCallBack.h"
#include"Transform.h"
#include"ColliderParentObject.h"
class ModelObject;
class LightManager;
class Camera;
class SlotMachine;
class TimeScale;
class Collider;
class CollisionManager;
class GameCamera;
class CollisionSphere;
class YoYo;
class Scaffold;
class ObjectManager;

class Player : public ColliderParentObject
{
	//モデルオブジェクト
	std::shared_ptr<ModelObject>m_modelObj;

	Vec3<float>m_oldPos;
	//移動
	Vec3<float>m_move = { 0,0,0 };

	//落下速度
	float m_fallSpeed;

	//接地フラグ（地面）
	bool m_isOnGround;
	//接地フラグ（足場）
	bool m_isOnScaffold;

	//ジャンプSE
	int m_jumpSE;

	//所持金
	CoinVault m_coinVault;

	//モデル全体を覆う球
	std::shared_ptr<CollisionSphere>m_bodySphereCol;

	//コライダー
	std::shared_ptr<Collider>m_bodyCollider;

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
	};
	std::shared_ptr<DamagedCallBack>m_damegedCallBack;

	//ブロックに触れた際のジャンプ権回復コールバック
	class CallBackWithBlock : public CollisionCallBack
	{
		Player* m_parent;
		std::weak_ptr<CollisionManager>m_collisionMgr;

		void OnCollisionEnter(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider)override {};

		void OnCollisionTrigger(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider)override;
	public:
		CallBackWithBlock(Player* arg_player, const std::weak_ptr<CollisionManager>& arg_collisionMgr)
			:m_parent(arg_player), m_collisionMgr(arg_collisionMgr) {}
	};
	std::shared_ptr<CallBackWithBlock>m_callBackWithBlock;

	//操作がキーボードかコントローラーか
	enum struct INPUT_CONFIG { KEY_BOARD, CONTROLLER };
	INPUT_CONFIG m_inputConfig = INPUT_CONFIG::CONTROLLER;

	//ヨーヨー
	std::shared_ptr<YoYo>m_yoYo;

	//向いている方向X
	float m_vecX;

	/* --- 足場から降りる動作 --- */
	//足場との当たり判定を取らない時間計測
	Timer m_stepDownTimer;
	//足場から降りているか
	bool m_stepDown;
	//足場から降りたときの落下スピード
	float m_stepDownFallSpeed = -0.06f;

	//ジャンプ
	void Jump(Vec3<float>* arg_rockOnPos = nullptr);

	//地面着地時に呼び出す
	void OnLanding(bool arg_isGround);
	
public:
	Player(std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<ObjectManager>arg_objMgr, std::weak_ptr<GameCamera>arg_cam);

	//初期化
	void Init(int arg_initHp,int arg_initCoinNum);
	//更新
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);
	//エフェクト描画
	void Draw2D(std::weak_ptr<Camera>arg_cam);

	//コインゲット関数
	void GetCoin(int arg_coinNum)
	{
		m_coinVault.Add(arg_coinNum);
	}

	//imguiデバッグ
	void ImguiDebug();

	//足場との当たり判定
	void HitCheckWithScaffold(const std::weak_ptr<Scaffold>arg_scaffold);

	//プレイヤーのモデル中央に合わせた座標ゲッタ
	Vec3<float>GetCenterPos()const;

	//ゲッタ
	const int GetCoinNum()const { return m_coinVault.GetNum(); }
	const int GetHp()const { return m_hp; }
};