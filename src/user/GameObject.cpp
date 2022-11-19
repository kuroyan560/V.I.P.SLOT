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
	//挙動制御初期化
	m_controller->OnInit(*this);

	//HPリセット
	m_hp = m_breed.lock()->m_maxHp;
}

void GameObject::Update(const TimeScale& arg_timeScale)
{
	m_controller->OnUpdate(*this, arg_timeScale);
}

void GameObject::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	m_controller->OnDraw(*this, arg_lightMgr, arg_cam);
}

int GameObject::Damage(int arg_amount)
{
	m_hp = std::max(0, m_hp - arg_amount);
	return m_controller->OnDamage(*this, arg_amount);
}

const int& GameObject::GetTypeID()
{
	return m_breed.lock()->m_typeID;
}

bool GameObject::IsDead()
{
	return m_controller->IsDead(*this);
}