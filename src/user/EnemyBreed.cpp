#include "EnemyBreed.h"
#include"Enemy.h"
#include"Model.h"
#include"ActPoint.h"
#include"LevelParameters.h"
#include"HitEffect.h"
#include"ColliderWithFloor.h"
#include"ModelAnimator.h"

EnemyBreed::DamgeTakenColliderCallBack EnemyBreed::s_damageCallBack(nullptr);

void EnemyBreed::DamgeTakenColliderCallBack::OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)
{
	assert(m_parent);

	//プレイヤーからの攻撃か
	if (!OtherCollider.lock()->CompareAttribute(COLLIDER_ATTRIBUTE::PLAYER_ATTACK))return;

	//被ダメージ
	m_parent->Damage(LevelParameters::GetPlayer().m_normalAttackDamage);

	//エフェクト
	HitEffect::Generate(Inter);
}

std::shared_ptr<Enemy>EnemyBreed::Generate()
{
	auto newEnemy = std::shared_ptr<Enemy>(new Enemy(*this));

	//系統クラスから攻撃パターンのクローンを受け取る
	if (m_control)
	{
		newEnemy->m_control = std::shared_ptr<EnemyControl>(m_control->Clone());
	}
	//アニメーション情報を持つならアニメーター生成
	if (!m_model->m_skelton->animations.empty())
	{
		newEnemy->m_animator = std::make_shared<ModelAnimator>(m_model);
	}

	//Breedの情報からActPoint生成
	for (auto& pointInfo : m_actPointInfos)
	{
		auto initTransform = pointInfo.m_initTransform;
		initTransform.SetParent(&newEnemy->m_transform);
		newEnemy->m_actPointArray.emplace_back(ActPoint::Generate(
			initTransform,
			true, true,
			pointInfo.m_maxMarkingCount));
	}

	//コライダーの生成
	s_damageCallBack.m_parent = newEnemy.get();
	for (auto& colliderOrigin : m_collidersOrigin)
	{
		auto clone = Collider::Clone(colliderOrigin, &newEnemy->m_transform);
		clone->SetActive(true);
		newEnemy->m_colliders.emplace_back(clone);
	}

	//床との当たり判定機構生成
	newEnemy->m_withFloorCollider = std::make_shared<ColliderWithFloor>(m_name, m_colliderWithFloorOffset, &newEnemy->m_transform);

	return newEnemy;
}
