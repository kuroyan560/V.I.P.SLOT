#include "Enemy.h"
#include"EnemyBreed.h"
#include"EnemyController.h"
#include"Collision.h"
#include"Collider.h"
#include"TimeScale.h"
#include"ConstParameters.h"

Enemy::Enemy(const std::shared_ptr<EnemyBreed>& arg_breed)
{
	//血統の記録
	m_breed = arg_breed;
	//挙動制御アタッチ
	m_controller = m_breed.lock()->m_controller->Clone();

	//血統よりコライダーをクローン
	for (auto& colOrigin : arg_breed->m_originCollider)
	{
		m_colliders.emplace_back(std::make_shared<Collider>(colOrigin->Clone(&m_transform, this)));
	}
}

void Enemy::Init()
{
	//所持コイン初期化
	m_coinVault.Set(m_breed.lock()->m_initCoinNum);

	//HP初期化
	m_hp = m_breed.lock()->m_maxHp;

	//挙動制御初期化
	m_controller->OnInit(*this);

	//被ダメージ無敵時間タイマーリセット
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
	//無敵時間中
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
