#include "Enemy.h"
#include"EnemyBreed.h"
#include"EnemyController.h"

void Enemy::Init(const std::shared_ptr<EnemyBreed>& arg_breed)
{
	//血統の記録
	m_breed = arg_breed;

	//所持コイン初期化
	m_coinVault.Set(arg_breed->m_initCoinNum);

	//HP初期化
	m_hp = arg_breed->m_maxHp;

	//挙動制御アタッチ
	m_controller = arg_breed->m_controller->Clone();
}

void Enemy::Update()
{
	m_controller->OnUpdate(*this);
}

void Enemy::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	m_controller->OnDraw(*this, arg_lightMgr, arg_cam);
}

const int& Enemy::GetTypeID()
{
	return m_breed.lock()->m_typeID;
}

bool Enemy::IsDead()
{
	return m_hp <= 0 || m_controller->IsDead(*this);
}
