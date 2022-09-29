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

	//既に寿命切れのコライダーを削除
	m_registerList.remove_if([](RegisterInfo& info) 
		{
			return info.m_collider;
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
				colA->m_isHit = true;
				colB->m_isHit = true;
				//コールバック関数呼び出し
				if (colA->m_callBack)colA->m_callBack->OnCollision(inter, itrB->m_myAttribute, *this);
				if (colB->m_callBack)colB->m_callBack->OnCollision(inter, itrA->m_myAttribute, *this);
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
