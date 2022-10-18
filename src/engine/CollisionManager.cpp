#include "CollisionManager.h"
#include"CollisionCallBack.h"
#include"Collider.h"

void CollisionManager::OnHit(const std::shared_ptr<Collider>& arg_myCollider, const std::shared_ptr<Collider>& arg_otherCollider, const Vec3<float>& arg_inter)
{
	arg_myCollider->m_isHit = true;

	//�R�[���o�b�N�֐��Ăяo��
	auto callBackList = arg_myCollider->m_callBackList.at(arg_otherCollider->m_tag);
	for (auto& callBack : callBackList)
	{
		callBack->OnCollisionEnter(arg_inter, arg_otherCollider, *this);
		if (!arg_myCollider->m_oldIsHit)callBack->OnCollisionTrigger(arg_inter, arg_otherCollider, *this);
	}
}

void CollisionManager::Update()
{
	//���Ɏ����؂�̃R���C�_�[���폜
	m_colliderList.remove_if([](std::shared_ptr<Collider> col)
	{
		return !col;
	});

	//�����蔻��L�^���Z�b�g
	for (auto& c : m_colliderList)
	{
		//���O�̃t���O�L�^
		c->m_oldIsHit = c->m_isHit;
		c->m_isHit = false;
	}

	//������Փ˔���
	for (auto itrA = m_colliderList.begin(); itrA != m_colliderList.end(); ++itrA)
	{
		auto colA = (*itrA);

		auto itrB = itrA;
		++itrB;
		for (; itrB != m_colliderList.end(); ++itrB)
		{
			auto colB = (*itrB);

			//���݂��ɃR�[���o�b�N�֐����p�ӂ���Ă��Ȃ��Ȃ�A�������Ă������N����Ȃ��̂Ŕ�����s���K�v�͖���
			if (colA->m_callBackList.find(colB->m_tag) == colA->m_callBackList.end())continue;
			if (colB->m_callBackList.find(colA->m_tag) == colB->m_callBackList.end())continue;

			Vec3<float>inter;
			if (colA->CheckHitCollision(colB, &inter))
			{
				OnHit(*itrA, *itrB, inter);
				OnHit(*itrB, *itrA, inter);
			}
		}
	}
}

void CollisionManager::DebugDraw(Camera& Cam)
{
	for (auto& c : m_colliderList)
	{
		if (!c->m_isActive)continue;
		c->DebugDraw(Cam);
	}
}

void CollisionManager::Register(const std::shared_ptr<Collider>& arg_collider)
{
	for (auto& c : m_colliderList)
	{
		//�o�^��
		if (*c == *arg_collider)
		{
			printf("Error : This collider ( %s ) has already registered.", arg_collider->m_name.c_str());
			assert(0);
		}
	}
	m_colliderList.emplace_back(arg_collider);
}

void CollisionManager::Remove(const std::shared_ptr<Collider>& arg_collider)
{
	m_colliderList.remove_if([arg_collider](std::shared_ptr<Collider> col)
		{
			return *col == *arg_collider;
		});
}