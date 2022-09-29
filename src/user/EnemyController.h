#pragma once
#include<memory>
class Enemy;
class TimeScale;
class LightManager;
class Camera;

class EnemyController
{
	friend class Enemy;

	virtual void OnInit(Enemy& arg_enemy) = 0;
	virtual void OnUpdate(Enemy& arg_enemy, const TimeScale& arg_timeScale) = 0;

	//デフォルトではただモデルを描画するだけ
	virtual void OnDraw(Enemy& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);

	//ダメージを受けたときの演出など
	virtual void OnDamage(Enemy& arg_enemy) {};

	//全ての動きを終えたときなど、EnemyController側から死亡フラグを立てられるように
	virtual bool IsDead(Enemy& arg_enemy) { return false; }

	//クローンの生成
	virtual std::unique_ptr<EnemyController>Clone() = 0;
};

//横移動
class EnemySlideMove : public EnemyController
{
	//スピード
	float m_xSpeed;

	//X軸方向スピード
	float m_xMove;

	void OnInit(Enemy& arg_enemy)override;
	void OnUpdate(Enemy& arg_enemy, const TimeScale& arg_timeScale)override;
	std::unique_ptr<EnemyController>Clone()override;
	bool IsDead(Enemy& arg_enemy)override;

public:
	EnemySlideMove(float arg_xSpeed) :m_xSpeed(arg_xSpeed) {}
};