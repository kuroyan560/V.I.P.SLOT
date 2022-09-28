#include "Enemy.h"
#include"EnemyBreed.h"
#include"EnemyController.h"

Enemy::Enemy(const std::shared_ptr<EnemyBreed>& arg_breed)
{
	//�����̋L�^
	m_breed = arg_breed;
	//��������A�^�b�`
	m_controller = m_breed.lock()->m_controller->Clone();
}

void Enemy::Init()
{
	//�����R�C��������
	m_coinVault.Set(m_breed.lock()->m_initCoinNum);

	//HP������
	m_hp = m_breed.lock()->m_maxHp;

	//�������䏉����
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
