#include "Enemy.h"
#include"EnemyBreed.h"
#include"EnemyController.h"

Enemy::Enemy(const std::shared_ptr<EnemyBreed>& arg_breed)
{
	//血統の記録
	m_breed = arg_breed;
	//挙動制御アタッチ
	m_controller = m_breed.lock()->m_controller->Clone();
}

void Enemy::Init()
{
	//所持コイン初期化
	m_coinVault.Set(m_breed.lock()->m_initCoinNum);

	//HP初期化
	m_hp = m_breed.lock()->m_maxHp;

	//挙動制御初期化
	m_controller->OnInit(*this);
}

void Enemy::Update()
{
	m_controller->OnUpdate(*this);
}

void Enemy::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	m_controller->OnDraw(*this, arg_lightMgr, arg_cam);
}

int Enemy::Damage(int arg_amount)
{
	m_hp -= arg_amount;
	m_controller->OnDamage(*this);
	return m_coinVault.GetNum();
}

const int& Enemy::GetTypeID()
{
	return m_breed.lock()->m_typeID;
}

bool Enemy::IsDead()
{
	return m_hp <= 0 || m_controller->IsDead(*this);
}
