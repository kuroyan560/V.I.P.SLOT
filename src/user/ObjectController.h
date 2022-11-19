#pragma once
#include<memory>
class GameObject;
class TimeScale;
class LightManager;
class Camera;

class ObjectController
{
	friend class GameObject;

protected:
	virtual void OnInit(GameObject& arg_enemy) = 0;
	virtual void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale) = 0;

	//デフォルトではただモデルを描画するだけ
	virtual void OnDraw(GameObject& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);

	//ダメージを受けたときの演出など（返り値：コイン数）
	virtual int OnDamage(GameObject& arg_enemy, int arg_damageAmount) { return 0; }

	//アタッチされているオブジェクトのHPが０以下か
	bool IsObjsHpdZero(GameObject& arg_obj)const;

	//全ての動きを終えたときなど、EnemyController側から死亡フラグを立てられるように
	virtual bool IsDead(GameObject& arg_enemy);

	//クローンの生成
	virtual std::unique_ptr<ObjectController>Clone() = 0;
};

//横移動
class ObjectSlideMove : public ObjectController
{
	//スピード
	float m_xSpeed;

	//X軸方向スピード
	float m_xMove;

	void OnInit(GameObject& arg_enemy)override;
	void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsDead(GameObject& arg_enemy)override;
	int OnDamage(GameObject& arg_enemy, int arg_damageAmount)override;

public:
	ObjectSlideMove(float arg_xSpeed) :m_xSpeed(arg_xSpeed) {}
};