#pragma once
#include<memory>
class Enemy;
class LightManager;
class Camera;

class EnemyController
{
	friend class Enemy;

	virtual void OnInit(Enemy& arg_enemy) = 0;
	virtual void OnUpdate(Enemy& arg_enemy) = 0;

	//デフォルトではただモデルを描画するだけ
	virtual void OnDraw(Enemy& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);

	//ダメージを受けたときの演出など
	virtual void OnDamage(Enemy& arg_enemy) {};

	//全ての動きを終えたときなど、EnemyController側から死亡フラグを立てられるように
	virtual bool IsDead(Enemy& arg_enemy) { return false; }

	//クローンの生成
	virtual std::unique_ptr<EnemyController>Clone() = 0;
};