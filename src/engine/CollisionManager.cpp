#include "CollisionManager.h"
#include"CollisionCallBack.h"
#include"Collider.h"

void CollisionManager::OnHit(const RegisterInfo& arg_myInfo, const RegisterInfo& arg_otherInfo, const Vec3<float>& arg_inter)
{
	const auto& col = arg_myInfo.m_collider;

	col->m_isHit = true;

	//コールバック関数呼び出し
	for (auto& callback : col->m_callBacks)
	{
		//相手が指定した振る舞いのときのみ、そのコールバックを呼び出す
		unsigned char attribute = callback.first;
		if (attribute & arg_otherInfo.m_myAttribute)
		{
			callback.second->OnCollision(arg_inter, arg_otherInfo.m_collider, arg_otherInfo.m_myAttribute, *this);
		}
	}
}

void CollisionManager::AddAttribute(std::string arg_attributeKey, unsigned char arg_attribute)
{
	m_attributeList[arg_attributeKey] = arg_attribute;
}

void CollisionManager::Update()
{
	//既に寿命切れのコライダーを削除
	m_registerList.remove_if([](RegisterInfo& info) 
		{
			return !info.m_collider;
		});

	//当たり判定記録リセット
	for (auto& info : m_registerList)info.m_collider->m_isHit = false;

	//総当り衝突判定
	for (auto itrA = m_registerList.begin(); itrA != m_registerList.end(); ++itrA)
	{
		auto colA = itrA->m_collider;

		auto itrB = itrA;
		++itrB;
		for (; itrB != m_registerList.end(); ++itrB)
		{
			auto colB = itrB->m_collider;

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
		//登録済
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
