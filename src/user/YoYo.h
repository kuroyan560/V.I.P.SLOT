#pragma once
#include<array>
#include<vector>
#include<memory>
#include"Transform.h"
#include"Timer.h"
class ModelObject;
class CollisionManager;
class CollisionSphere;
class Collider;
class TimeScale;
class LightManager;
class Camera;

class YoYo
{
	//ヨーヨーモデル
	std::shared_ptr<ModelObject>m_modelObj;

	//向き指定用トランスフォーム
	Transform m_vecTransform;

	//コライダー
	//N攻撃
	std::shared_ptr<CollisionSphere>m_neutralColSphere;
	std::shared_ptr<Collider>m_neutralCol;
	//投擲
	std::shared_ptr<CollisionSphere>m_throwColSphere;
	std::shared_ptr<Collider>m_throwCol;


	//状態遷移
	enum STATUS
	{
		THROW_0,	//投げ_0
		THROW_1,	//投げ_1
		THROW_2,	//投げ_2
		NEUTRAL,	//N攻撃
		STATUS_NUM,
		HAND
	}m_status;

	//ステータスごとの設定項目
	struct StatusParameter
	{
		//再生するアニメーション名
		std::string m_animName = "";
		//ステータス終了時間
		int m_finishInterval = 60;
		//中断入力が可能になる時間
		int m_interruptInterval = 60;
	};
	std::array<StatusParameter, STATUS_NUM>m_statusParams;

	//予約入力
	bool m_previousInput = false;

	//ステータス終了計測用タイマー
	Timer m_timer;

	//現在の攻撃が中断可能か
	bool CanInterrupt()
	{
		return (float)m_statusParams[(int)m_status].m_interruptInterval < m_timer.GetElaspedTime();
	}

public:

	YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform);

	/// <summary>
	/// ヨーヨーの固定パラメータ設定（≠初期化）
	/// </summary>
	/// <param name="arg_neutralColSphereRadius">当たり判定用球の半径（N攻撃）</param>
	/// <param name="arg_throwColSphereRadius">当たり判定用球の半径（投擲）</param>
	void Awake(float arg_neutralColSphereRadius, float arg_throwColSphereRadius);

	//初期化
	void Init();
	//更新
	void Update(const TimeScale& arg_timeScale, float arg_playersVecX);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//imguiデバッグ
	void AddImguiDebugItem();

	//投擲
	void Throw(float arg_vecX);

	//N攻撃
	void Neutral();

	//攻撃中か
	bool IsActive() { return m_status != HAND; }
};