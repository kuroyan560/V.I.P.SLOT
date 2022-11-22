#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
class GameObject;
class TimeScale;
class LightManager;
class Camera;

class ObjectController
{
	friend class GameObject;

protected:
	//純粋仮想関数（アレンジ必須）
	//初期化
	virtual void OnInit(GameObject& arg_enemy) = 0;
	//更新
	virtual void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale) = 0;
	//クローンの生成
	virtual std::unique_ptr<ObjectController>Clone() = 0;

	//仮想関数（アレンジ可能）
	//デフォルトではただモデルを描画するだけ
	virtual void OnDraw(GameObject& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);
	//ダメージを受けたときの演出など（返り値：コイン数）
	virtual int OnDamage(GameObject& arg_enemy, int arg_damageAmount) { return 0; }
	//退場したか
	virtual bool IsLeave(GameObject& arg_obj)const { return false; }

	//その他（関数固定）
	//アタッチされているオブジェクトのHPが０以下か
	bool IsObjsHpZero(GameObject& arg_obj)const;
	//全ての動きを終えたときなど、EnemyController側から死亡フラグを立てられるように
	bool IsDead(GameObject& arg_obj) { return IsLeave(arg_obj) || IsObjsHpZero(arg_obj); }


};

//横移動
class OC_SlideMove : public ObjectController
{
	//スピード
	float m_xSpeed;

	//X軸方向スピード
	float m_xMove;

	//座標Y
	float m_posY;

	void OnInit(GameObject& arg_enemy)override;
	void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override;

public:
	OC_SlideMove(float arg_xSpeed)
	{
		m_xSpeed = arg_xSpeed;
	}
	void SetPosY(float arg_posY) { m_posY = arg_posY; }
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

	void OnInit(GameObject& arg_enemy)override;
	void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override;

	//近い端のXを返す
	float GetNearEdgePosX(float arg_posX)const;

public:
	OC_SlimeBattery(float* arg_intervalTimes, float arg_jumpPower) :m_jumpPower(arg_jumpPower)
	{
		for (int i = 0; i < STATUS::NUM; ++i)m_intervalTimes[i] = arg_intervalTimes[i];
	}

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