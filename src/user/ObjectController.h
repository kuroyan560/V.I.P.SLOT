#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
#include"Angle.h"
#include"KuroMath.h"
class GameObject;
class ObjectManager;
class CollisionManager;
class TimeScale;
class LightManager;
class Camera;

class ObjectController
{
	friend class GameObject;

protected:
	//オブジェクトマネージャ
	static std::weak_ptr<ObjectManager>s_objMgr;

public:
	static void AttachObjectManager(std::weak_ptr<ObjectManager>arg_objMgr)
	{
		s_objMgr = arg_objMgr;
	}

protected:
	//純粋仮想関数（アレンジ必須）
	//初期化
	virtual void OnInit(GameObject& arg_obj) = 0;
	//更新
	virtual void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr) = 0;
	//クローンの生成
	virtual std::unique_ptr<ObjectController>Clone() = 0;

	//仮想関数（アレンジ可能）
	//デフォルトではただモデルを描画するだけ
	virtual void OnDraw(GameObject& arg_obj, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);
	//ダメージを受けたときの演出など（返り値：コイン数）
	virtual int OnDamage(GameObject& arg_obj, int arg_damageAmount) { return 0; }
	//退場したか
	virtual bool IsLeave(GameObject& arg_obj)const { return false; }

	//その他（関数固定）
	//アタッチされているオブジェクトのHPが０以下か
	bool IsObjsHpZero(GameObject& arg_obj)const;
	//全ての動きを終えたときなど、EnemyController側から死亡フラグを立てられるように
	bool IsDead(GameObject& arg_obj)const { return IsLeave(arg_obj) || IsObjsHpZero(arg_obj); }
	//画面外に出たか
	bool IsOutOfScreen(GameObject& arg_obj)const;
};

//指定した方向に移動
class OC_DirectionMove : public ObjectController
{
	//スタート位置
	Vec3<float>m_startPos;

	//XY平面移動方向
	Vec2<float>m_moveDirXY;

	//スピード
	float m_speed = 1.0f;

	//蛇行する際のラジアン
	Angle m_radian;
	//蛇行するか
	bool m_sinMeandeling = false;
	//蛇行のインターバル
	float m_meandelingInterval = 60.0f;
	//蛇行の角度
	Angle m_meandelingAngle = Angle(30);

	void OnInit(GameObject& arg_obj)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override { return IsOutOfScreen(arg_obj); }

public:
	/// <summary>
	/// パラメータ設定
	/// </summary>
	/// <param name="arg_startPos">初期位置</param>
	void SetParameters(Vec3<float>arg_startPos) { m_startPos = arg_startPos; }

	/// <summary>
	/// パラメータ設定
	/// </summary>
	/// <param name="arg_startPos">初期位置</param>
	/// <param name="arg_moveDirXY">XY平面移動方向</param>
	/// <param name="arg_speed">移動スピード</param>
	/// <param name="arg_sinMeandeling">蛇行するか（sinカーブ）</param>
	/// <param name="arg_meandelingInterval">蛇行のインターバル</param>
	/// <param name="arg_meandelingAngle">蛇行の角度</param>
	void SetParameters(
		Vec3<float>arg_startPos,
		Vec2<float>arg_moveDirXY,
		float arg_speed,
		bool arg_sinMeandeling = false,
		float arg_meandelingInterval = 60.0f,
		Angle arg_meandelingAngle = Angle(30))
	{
		m_startPos = arg_startPos;
		m_moveDirXY = arg_moveDirXY;
		m_speed = arg_speed;
		m_sinMeandeling = arg_sinMeandeling;
		m_meandelingInterval = arg_meandelingInterval;
		m_meandelingAngle = arg_meandelingAngle;
	}
};

//指定した位置にイージング関数を用いて移動
class OC_DestinationEaseMove : public ObjectController
{
	//イージング種別
	EASE_CHANGE_TYPE m_easeChangeType;
	EASING_TYPE m_easeType;
	//スタート地点
	Vec3<float>m_startPos;
	//目的地
	Vec3<float>m_destinationPos;
	//時間
	float m_interval;
	//タイマー
	Timer m_timer;

	void OnInit(GameObject& arg_obj)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override { return m_timer.IsTimeUp(); }

public:
	OC_DestinationEaseMove(EASE_CHANGE_TYPE arg_easeChangeType,EASING_TYPE arg_easeType, float arg_interval)
		:m_easeChangeType(arg_easeChangeType),m_easeType(arg_easeType), m_interval(arg_interval) {}

	/// <summary>
	/// パラメータ設定
	/// </summary>
	/// <param name="arg_startPos">スタート地点</param>
	/// <param name="arg_destinationPos">目的地</param>
	void SetParameters(Vec3<float>arg_startPos, Vec3<float>arg_destinationPos)
	{
		m_startPos = arg_startPos;
		m_destinationPos = arg_destinationPos;
	}
};

//飛び跳ね＆弾を発射（スライム砲台、画面外から登場後飛び跳ね＆ショットで移動）
class OC_SlimeBattery : public ObjectController
{
public:
	//ステータス管理
	enum STATUS { APPEAR, WAIT, JUMP_AND_SHOT, DISAPPEAR, NUM };

private:
	STATUS m_status;
	//時間指定
	std::array<float, STATUS::NUM>m_intervalTimes;

	//時間計測
	Timer m_timer;

	//移動量Y
	float m_moveY;

	//登場時のY
	float m_appearY = 3.0f;

	//ジャンプ力
	float m_jumpPower = 2.0f;

	//出発地のX軸座標
	float m_departureX = 0.0f;

	//目的地のX軸座標
	float m_destinationX = 0.0f;

	//飛び跳ね位置
	std::vector<float>m_spotXList;
	//現在の座標基準Xインデックス
	int m_spotXIdx = 0;

	void OnInit(GameObject& arg_obj)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override;

	//近い端のXを返す
	float GetNearEdgePosX(float arg_posX)const;

public:
	OC_SlimeBattery(float* arg_intervalTimes, float arg_jumpPower) :m_jumpPower(arg_jumpPower)
	{
		for (int i = 0; i < STATUS::NUM; ++i)m_intervalTimes[i] = arg_intervalTimes[i];
	}

	/// <summary>
	/// パラメータ設定
	/// </summary>
	/// <param name="arg_appearY">登場時のY座標</param>
	/// <param name="arg_destinationXArray">目標地点X座標配列</param>
	/// <param name="arg_arraySize">配列の要素数</param>
	void SetParameters(float arg_appearY, float arg_destinationXArray[], size_t arg_arraySize)
	{
		m_spotXList.clear();
		for (int i = 0; i < static_cast<int>(arg_arraySize); ++i)
		{
			m_spotXList.emplace_back(arg_destinationXArray[i]);
		}
		m_appearY = arg_appearY;
	}
};