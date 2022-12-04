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
	virtual void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos) = 0;
	//更新
	virtual void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr) = 0;
	//クローンの生成
	virtual std::unique_ptr<ObjectController>Clone() = 0;

	//仮想関数（アレンジ可能）
	//デフォルトではただモデルを描画するだけ
	virtual void OnDraw(GameObject& arg_obj, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);
	//ダメージを受けたときの演出など
	virtual void OnDamage(GameObject& arg_obj, int arg_damageAmount) {}
	//退場したか
	virtual bool IsLeave(GameObject& arg_obj)const { return false; }

	//その他（関数固定）
	//アタッチされているオブジェクトのHPが０以下か
	bool IsObjsHpZero(GameObject& arg_obj)const;
	//全ての動きを終えたときなど、EnemyController側から死亡フラグを立てられるように
	bool IsDead(GameObject& arg_obj)const { return IsLeave(arg_obj) || IsObjsHpZero(arg_obj); }
	//画面外に出たか
	bool IsOutOfScreen(GameObject& arg_obj)const;
	//対象オブジェクトのローカルタイムスケール
	float GetLocalTimeScale(GameObject& arg_obj)const;
};

//指定した方向に移動
class OC_DirectionMove : public ObjectController
{
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

	void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override { return IsOutOfScreen(arg_obj); }

public:
	/// <summary>
	/// パラメータ設定
	/// </summary>
	/// <param name="arg_moveDirXY">XY平面移動方向</param>
	/// <param name="arg_speed">移動スピード</param>
	/// <param name="arg_sinMeandeling">蛇行するか（sinカーブ）</param>
	/// <param name="arg_meandelingInterval">蛇行のインターバル</param>
	/// <param name="arg_meandelingAngle">蛇行の角度</param>
	void SetParameters(
		Vec2<float>arg_moveDirXY,
		float arg_speed,
		bool arg_sinMeandeling = false,
		float arg_meandelingInterval = 60.0f,
		Angle arg_meandelingAngle = Angle(30))
	{
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
protected:
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
	//ゴールしているか
	bool m_isGoal = false;

	void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override { return m_isGoal; }

public:
	OC_DestinationEaseMove(EASE_CHANGE_TYPE arg_easeChangeType,EASING_TYPE arg_easeType, float arg_interval)
		:m_easeChangeType(arg_easeChangeType),m_easeType(arg_easeType), m_interval(arg_interval) {}

	/// <summary>
	/// パラメータ設定
	/// </summary>
	/// <param name="arg_destinationPos">目的地</param>
	void SetParameters(Vec3<float>arg_destinationPos)
	{
		m_destinationPos = arg_destinationPos;
	}
};

//指定した対象(GameObject)に向かってイージング関数を用いて移動
class OC_TargetObjectEaseMove : public OC_DestinationEaseMove
{
	//対象となるゲームオブジェクト
	GameObject* m_target;

	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;

public:
	OC_TargetObjectEaseMove(EASE_CHANGE_TYPE arg_easeChangeType, EASING_TYPE arg_easeType, float arg_interval)
		:OC_DestinationEaseMove(arg_easeChangeType, arg_easeType, arg_interval) {}

	/// <summary>
	/// パラメータ設定
	/// </summary>
	/// <param name="arg_destinationPos">目的地</param>
	void SetParameters(GameObject* arg_target);
};

//飛び跳ね＆弾を発射（スライム砲台、画面外から登場後飛び跳ね＆ショットで移動）
class OC_SlimeBattery : public ObjectController
{
public:
	//ステータス管理
	enum STATUS { WAIT, JUMP_AND_SHOT, NUM };

private:
	STATUS m_status;

	//時間計測
	Timer m_timer;

	//移動量Y
	float m_moveY;

	//出発地のX軸座標
	float m_departureX = 0.0f;

	//目的地のX軸座標
	float m_destinationX = 0.0f;

	//退場フラグ
	bool m_isLeave = false;

	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	bool IsLeave(GameObject& arg_obj)const override
	{
		return m_isLeave;
	}

protected:
	OC_SlimeBattery() {}

	virtual std::unique_ptr<ObjectController>Clone() = 0;
	virtual void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override;
	//飛び跳ね時に呼び出す処理
	virtual void OnSlimeJump() = 0;
	/// <summary>
	/// 次の目的地X決定
	/// </summary>
	/// <param name="arg_obj">アタッチされているオブジェクト</param>
	/// <param name="arg_decideNextX">目的地Xを格納するための変数</param>
	/// <returns>目的地があるか</returns>
	virtual bool OnDecideNextDetinationX(GameObject& arg_obj, float& arg_decideNextX) = 0;

public:
	virtual ~OC_SlimeBattery() {}
};

//飛び跳ね＆弾を発射（スライム砲台、画面外から登場後飛び跳ね＆ショットで移動）※その場で。X移動しない
class OC_SlimeBattery_FixedX : public OC_SlimeBattery
{
	int m_countMax = 3;
	int m_count;

	std::unique_ptr<ObjectController>Clone()override
	{
		return std::make_unique<OC_SlimeBattery_FixedX>();
	}
	void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override
	{
		OC_SlimeBattery::OnInit(arg_obj, arg_initPos);
		m_count = m_countMax;
	}
	void OnSlimeJump()override {}
	bool OnDecideNextDetinationX(GameObject& arg_obj, float& arg_decideNextX)override;

public:
	//飛び跳ね回数
	void SetJumpCountMax(int arg_countMax) { m_countMax = arg_countMax; }
};


//飛び跳ね＆弾を発射（スライム砲台、画面外から登場後飛び跳ね＆ショットで移動）※ルート指定
class OC_SlimeBattery_RouteDefined : public OC_SlimeBattery
{
	//飛び跳ね位置
	std::vector<float>m_spotXList;
	//現在の座標基準Xインデックス
	int m_spotXIdx = 0;

	std::unique_ptr<ObjectController>Clone()override
	{
		return std::make_unique<OC_SlimeBattery_RouteDefined>();
	}
	void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override;
	void OnSlimeJump()override
	{
		m_spotXIdx++;
	}
	bool OnDecideNextDetinationX(GameObject& arg_obj, float& arg_decideNextX)override
	{
		//次の目的地が存在するか
		bool existNext = m_spotXIdx < static_cast<int>(m_spotXList.size());
		if (existNext)
		{
			//インデックスをインクリメントして次の目的地X指定
			arg_decideNextX = m_spotXList[m_spotXIdx];
		}
		return existNext;
	}

public:
	/// <summary>
	/// X軸方向ルート指定
	/// </summary>
	/// <param name="arg_destinationXArray">目標地点X座標配列</param>
	/// <param name="arg_arraySize">配列の要素数</param>
	void SetRouteX(float arg_destinationXArray[], size_t arg_arraySize)
	{
		m_spotXList.clear();
		for (int i = 0; i < static_cast<int>(arg_arraySize); ++i)
		{
			m_spotXList.emplace_back(arg_destinationXArray[i]);
		}
	}
};

//飛び跳ね＆弾を発射（スライム砲台、画面外から登場後飛び跳ね＆ショットで移動）※プレイヤーを追う
class OC_SlimeBattery_ChasePlayer : public OC_SlimeBattery
{

};
