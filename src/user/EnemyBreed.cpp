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

	//�v���C���[����̍U����
	if (!OtherCollider.lock()->CompareAttribute(COLLIDER_ATTRIBUTE::PLAYER_ATTACK))return;

	//��_���[�W
	m_parent->Damage(LevelParameters::GetPlayer().m_normalAttackDamage);

	//�G�t�F�N�g
	HitEffect::Generate(Inter);
}

std::shared_ptr<Enemy>EnemyBreed::Generate()
{
	auto newEnemy = std::shared_ptr<Enemy>(new Enemy(*this));

	//�n���N���X����U���p�^�[���̃N���[�����󂯎��
	if (m_control)
	{
		newEnemy->m_control = std::shared_ptr<EnemyControl>(m_control->Clone());
	}
	//�A�j���[�V�����������Ȃ�A�j���[�^�[����
	if (!m_model->m_skelton->animations.empty())
	{
		newEnemy->m_animator = std::make_shared<ModelAnimator>(m_model);
	}

	//Breed�̏�񂩂�ActPoint����
	for (auto& pointInfo : m_actPointInfos)
	{
		auto initTransform = pointInfo.m_initTransform;
		initTransform.SetParent(&newEnemy->m_transform);
		newEnemy->m_actPointArray.emplace_back(ActPoint::Generate(
			initTransform,
			true, true,
			pointInfo.m_maxMarkingCount));
	}

	//�R���C�_�[�̐���
	s_damageCallBack.m_parent = newEnemy.get();
	for (auto& colliderOrigin : m_collidersOrigin)
	{
		auto clone = Collider::Clone(colliderOrigin, &newEnemy->m_transform);
		clone->SetActive(true);
		newEnemy->m_colliders.emplace_back(clone);
	}

	//���Ƃ̓����蔻��@�\����
	newEnemy->m_withFloorCollider = std::make_shared<ColliderWithFloor>(m_name, m_colliderWithFloorOffset, &newEnemy->m_transform);

	return newEnemy;
}
