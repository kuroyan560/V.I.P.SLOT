#include "Enemy.h"
#include"EnemyBreed.h"
#include"EnemyController.h"
#include"Collision.h"
#include"Collider.h"
#include"TimeScale.h"
#include"ConstParameters.h"

Enemy::Enemy(const std::shared_ptr<EnemyBreed>& arg_breed)
{
	//�����̋L�^
	m_breed = arg_breed;
	//��������A�^�b�`
	m_controller = m_breed.lock()->m_controller->Clone();

	//�������R���C�_�[���N���[��
	for (auto& colOrigin : arg_breed->m_originCollider)
	{
		m_colliders.emplace_back(std::make_shared<Collider>(colOrigin->Clone(&m_transform, this)));
	}
}

void Enemy::Init()
{
	//�����R�C��������
	m_coinVault.Set(m_breed.lock()->m_initCoinNum);

	//HP������
	m_hp = m_breed.lock()->m_maxHp;

	//�������䏉����
	m_controller->OnInit(*this);

	//��_���[�W���G���ԃ^�C�}�[���Z�b�g
	m_damagedInvincibleTimer.Reset(0);
}

void Enemy::Update(const TimeScale& arg_timeScale)
{
	m_controller->OnUpdate(*this, arg_timeScale);

	m_damagedInvincibleTimer.UpdateTimer(arg_timeScale.GetTimeScale());
}

void Enemy::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	m_controller->OnDraw(*this, arg_lightMgr, arg_cam);
}

int Enemy::Damage(int arg_amount)
{
	//���G���Ԓ�
	if (!m_damagedInvincibleTimer.IsTimeUp())return 0;

	m_hp -= arg_amount;
	m_controller->OnDamage(*this);
	m_damagedInvincibleTimer.Reset(ConstParameter::Enemy::INVINCIBLE_TIME_WHEN_DAMAGED);

	printf("Enemy : Damaged : remain hp %d\n", m_hp);

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
