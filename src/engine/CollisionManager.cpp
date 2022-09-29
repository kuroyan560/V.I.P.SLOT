#include "CollisionManager.h"
#include"Collider.h"
#include"CollisionCallBack.h"

void CollisionManager::CheckInvalid()
{
	if (!m_invalid)return;

	printf("Invalid : This CollisionManager hasn't be initialized yet.");
	assert(0);
}

void CollisionManager::Generate(const std::map<std::string, unsigned char>& arg_attributeList)
{
	m_attributeList = arg_attributeList;
	m_invalid = false;
}

void CollisionManager::Update()
{
	CheckInvalid();

	//���Ɏ����؂�̃R���C�_�[���폜
	m_registerList.remove_if([](RegisterInfo& info) 
		{
			return info.m_collider;
		});

	//�����蔻��L�^���Z�b�g
	for (auto& info : m_registerList)info.m_collider->m_isHit = false;

	//������Փ˔���
	for (auto itrA = m_registerList.begin(); itrA != m_registerList.end(); ++itrA)
	{
		auto itrB = itrA;
		++itrB;
		for (; itrB != m_registerList.end(); ++itrB)
		{
			Vec3<float>inter;
			if (itrA->m_collider->CheckHitCollision(itrB->m_collider, &inter))
			{
				itrA->m_collider->m_isHit = true;
				itrB->m_collider->m_isHit = true;
				//�R�[���o�b�N�֐��Ăяo��
				if (itrA->m_callBack)itrA->m_callBack->OnCollision(inter, itrA->m_collider, itrB->m_collider);
				if (itrB->m_callBack)itrB->m_callBack->OnCollision(inter, itrB->m_collider, itrA->m_collider);
			}
		}
	}
}

void CollisionManager::DebugDraw(Camera& Cam)
{
	CheckInvalid();

	for (auto& info : m_registerList)
	{
		if (!info.m_collider->m_isActive)continue;
		info.m_collider->DebugDraw(Cam);
	}
}
