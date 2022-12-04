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

void GameObject::Init(Vec3<float>arg_initPos)
{
	//挙動制御初期化
	m_controller->OnInit(*this, arg_initPos);

	//HPリセット
	m_hp = m_breed.lock()->m_maxHp;

	//描画パラメータ初期化
	m_drawParam = m_breed.lock()->m_drawParam;

	//ダメージタイマーリセット
	m_damageTimer.Reset(0.0f);

	//親リセット
	SetParentObj(nullptr);
}

void GameObject::Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//挙動更新
	m_controller->OnUpdate(*this, arg_timeScale, arg_collisionMgr);
	//親が死んたら親設定をはずす
	if (m_parentObj && m_parentObj->IsDead())m_parentObj = nullptr;

	//ダメージ演出
	if (m_damageTimer.UpdateTimer(arg_timeScale.GetTimeScale()))
	{
		//コライダーをアクティブに
		for (auto& col : m_colliders)col->SetActive(true);
	}
	//ダメージ点滅
	m_drawParam.m_fillColor.m_a = KuroMath::Ease(Out, Circ, m_damageTimer.GetTimeRate(), 1.0f, 0.0f);
}

void GameObject::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	m_controller->OnDraw(*this, arg_lightMgr, arg_cam);
}

int GameObject::Damage(int arg_amount)
{
	m_hp = std::max(0, m_hp - arg_amount);
	m_controller->OnDamage(*this, arg_amount);

	//死亡
	if (m_hp <= 0)return m_breed.lock()->m_killCoinNum;

	//まだ生きている
	m_damageTimer.Reset(30.0f);
	m_drawParam.m_fillColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

	//コライダーを非アクティブに
	for (auto& col : m_colliders)col->SetActive(false);

	return 0;
}

const int& GameObject::GetTypeID()
{
	return m_breed.lock()->m_typeID;
}

bool GameObject::IsDead()
{
	return m_controller->IsDead(*this);
}