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
class CollisionCallBack;

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
		//最大加速度スカラー
		Vec3<float>m_maxAccelVal = { 0,0,0 };
		//攻撃のインターバル
		int m_attackGapInterval = 0;
	};
	std::array<StatusParameter, STATUS_NUM>m_statusParams;

	//3連撃の予約入力
	bool m_tripleAttackPreviousInput = false;

	//ステータス終了計測用タイマー
	Timer m_timer;

	//攻撃の勢いによる加速度
	Vec3<float>m_accel;
	//攻撃トリガー時の加速度（初期値）
	Vec3<float>m_iniAccel;

	//インターバル（攻撃の隙）
	Timer m_attackGapInterval;
	//インターバル中の予約入力種別
	enum PREVIOUS_INPUT { NONE_INPUT, THROW_INPUT, NEUTRAL_INPUT };
	//インターバル中の予約入力
	PREVIOUS_INPUT m_attackGapPreviousInput = NONE_INPUT;
	//インターバル中の予約入力を受け付けるタイミング（残り時間）
	int m_canAttackGapPreviousInputLeftTime = 10;

	//現在の攻撃が中断可能か
	bool CanInterrupt()
	{
		return (float)m_statusParams[(int)m_status].m_interruptInterval < m_timer.GetElaspedTime();
	}
	//インターバル（攻撃の隙）中か
	bool IsAttackGapInterval(PREVIOUS_INPUT arg_input);

	//ステータスの動的更新時に呼び出す
	void StatusInit(Vec3<float>arg_accelVec);

public:
	YoYo(
		std::weak_ptr<CollisionManager>arg_collisionMgr,
		Transform* arg_playerTransform,
		std::weak_ptr<CollisionCallBack>arg_attackCallBack,
		std::weak_ptr<CollisionCallBack>arg_parryCallBack);

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

	/// <summary>
	/// 投擲
	/// </summary>
	/// <param name="arg_vecX">X軸に対する投擲方向</param>
	/// <returns>成功したか</returns>
	bool Throw(float arg_vecX);

	/// <summary>
	/// N攻撃
	/// </summary>
	/// <returns>成功したか</returns>
	bool Neutral();

	//投擲中か
	bool IsThrow()const { return THROW_0 <= m_status && m_status <= THROW_2; }

	//攻撃中か
	bool IsInvincible()const { return m_status == NEUTRAL; }

	//空中状態か（落下速度加算するか）
	bool IsAir()const { return m_status == THROW_0 || m_status == THROW_1; }

	//勢いの加速度ゲッタ
	const float& GetAccelX()const { return m_accel.x; }
	const float& GetAccelY()const { return m_accel.y; }
};