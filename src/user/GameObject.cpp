#include"GameObject.h"
#include"ObjectBreed.h"
#include"ObjectController.h"
#include"Collision.h"
#include"Collider.h"
#include"TimeScale.h"
#include"ConstParameters.h"

GameObject::GameObject(const std::shared_ptr<ObjectBreed>& arg_breed)
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

void GameObject::Init()
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

void GameObject::Update(const TimeScale& arg_timeScale)
{
	m_controller->OnUpdate(*this, arg_timeScale);

	//被ダメージ時の無敵時間計測
	m_damagedInvincibleTimer.UpdateTimer(arg_timeScale.GetTimeScale());
}

void GameObject::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	m_controller->OnDraw(*this, arg_lightMgr, arg_cam);
}

int GameObject::Damage(int arg_amount)
{
	using namespace ConstParameter::GameObject;

	//m_hp -= arg_amount;
	m_controller->OnDamage(*this);

	//被ダメージ時一定時間無敵に
	m_damagedInvincibleTimer.Reset(INVINCIBLE_TIME_ON_DAMAGED);

	printf("GameObject : Damaged : remain hp %d\n", m_hp);

	return m_coinVault.GetNum();
}

const int& GameObject::GetTypeID()
{
	return m_breed.lock()->m_typeID;
}

bool GameObject::IsDead()
{
	return m_hp <= 0 || m_controller->IsDead(*this);
}

bool GameObject::IsKilled()
{
	return m_hp <= 0;
}
