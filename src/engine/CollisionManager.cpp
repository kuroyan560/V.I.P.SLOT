#include "CollisionManager.h"
#include"CollisionCallBack.h"
#include"Collider.h"

void CollisionManager::OnHit(const RegisterInfo& arg_myInfo, const RegisterInfo& arg_otherInfo, const Vec3<float>& arg_inter)
{
	const auto& col = arg_myInfo.m_collider;

	col->m_isHit = true;

	//�R�[���o�b�N�֐��Ăяo��
	for (auto& callback : col->m_callBacks)
	{
		//���肪�w�肵���U�镑���̂Ƃ��̂݁A���̃R�[���o�b�N���Ăяo��
		unsigned char attribute = callback.first;
		if (attribute & arg_otherInfo.m_myAttribute)
		{
			callback.second->OnCollisionEnter(arg_inter, arg_otherInfo.m_collider, *this);

			//�u��
			if (!col->m_oldIsHit)callback.second->OnCollisionTrigger(arg_inter, arg_otherInfo.m_collider, *this);
		}
	}
}

void CollisionManager::AddAttribute(std::string arg_attributeKey, unsigned char arg_attribute)
{
	m_attributeList[arg_attributeKey] = arg_attribute;
}

void CollisionManager::Update()
{
	//���Ɏ����؂�̃R���C�_�[���폜
	m_registerList.remove_if([](RegisterInfo& info) 
		{
			return !info.m_collider;
		});

	//�����蔻��L�^���Z�b�g
	for (auto& info : m_registerList)
	{
		//���O�̃t���O�L�^
		info.m_collider->m_oldIsHit = info.m_collider->m_isHit;
		info.m_collider->m_isHit = false;
	}

	//������Փ˔���
	for (auto itrA = m_registerList.begin(); itrA != m_registerList.end(); ++itrA)
	{
		auto colA = itrA->m_collider;
		//�R���C�_�[���A�N�e�B�u��ԂłȂ�
		if (!colA->m_isActive)continue;

		auto itrB = itrA;
		++itrB;
		for (; itrB != m_registerList.end(); ++itrB)
		{
			auto colB = itrB->m_collider;
			//�R���C�_�[���A�N�e�B�u��ԂłȂ�
			if (!colB->m_isActive)continue;

			//���݂��ɃR�[���o�b�N�֐����p�ӂ���Ă��Ȃ��Ȃ�A�������Ă������N����Ȃ��̂Ŕ�����s���K�v�͖���
			if (colA->m_callBacks.find(itrB->m_myAttribute) == colA->m_callBacks.end()
				&& colB->m_callBacks.find(itrA->m_myAttribute) == colB->m_callBacks.end())continue;

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
	for (auto& info : m_registerList)
	{
		if (!info.m_collider->m_isActive)continue;
		info.m_collider->DebugDraw(Cam);
	}
}

void CollisionManager::Register(std::string arg_attributeKey, const std::shared_ptr<Collider>& arg_collider)
{
	for (auto& info : m_registerList)
	{
		//�o�^��
		if (*info.m_collider == *arg_collider)
		{
			printf("Error : This collider ( %s ) has already registered.", arg_collider->m_name.c_str());
			assert(0);
		}
	}

	auto attribute = GetAttribute(arg_attributeKey);
	RegisterInfo info;
	info.m_myAttribute = attribute;
	info.m_collider = arg_collider;
	m_registerList.push_back(info);
}

void CollisionManager::Remove(const std::shared_ptr<Collider>& arg_collider)
{
	m_registerList.remove_if([arg_collider](RegisterInfo info)
		{
			return *info.m_collider == *arg_collider;
		});
}

const unsigned char& CollisionManager::GetAttribute(std::string arg_key) const
{
	auto result = m_attributeList.find(arg_key);
	if (result == m_attributeList.end())
	{
		printf("Error : \" %s \" isn't registered for the key of collider's attribute.", arg_key.c_str());
		assert(0);
	}
	return m_attributeList.at(arg_key);
}
