#pragma once
#include<memory>
#include<array>
#include<forward_list>
#include"ConstParameters.h"

class EnemyBreed;
class Enemy;
class TimeScale;
class LightManager;
class Camera;

//エネミーの定義、管理を行う
class EnemyManager
{
	using ENEMY_TYPE = ConstParameter::Enemy::TYPE;

	//血統
	std::array<std::shared_ptr<EnemyBreed>, static_cast<int>(ENEMY_TYPE::NUM)>m_breeds;

	//全エネミー配列
	std::array<std::forward_list<std::shared_ptr<Enemy>>, static_cast<int>(ENEMY_TYPE::NUM)>m_enemys;

	//死亡しているエネミー配列
	std::array<std::forward_list<std::shared_ptr<Enemy>>, static_cast<int>(ENEMY_TYPE::NUM)>m_deadEnemyArray;
	//生存しているエネミー配列
	std::array<std::forward_list<std::shared_ptr<Enemy>>, static_cast<int>(ENEMY_TYPE::NUM)>m_aliveEnemyArray;
public:
	EnemyManager();
	void Init();
	void Update(const TimeScale& arg_timeScale);
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	void Appear(ENEMY_TYPE arg_type);
};

