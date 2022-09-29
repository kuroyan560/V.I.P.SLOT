#include "Collider.h"

int Collider::s_id = 0;

Collider::Collider(const std::string& arg_name, 
	const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray, 
	const std::shared_ptr<CollisionCallBack>& arg_callBack)
	:m_id(s_id++), m_name(arg_name), m_primitiveArray(arg_primitiveArray),m_callBack(arg_callBack)
{
}

Collider Collider::Clone(Transform* arg_parent)
{
	std::vector<std::shared_ptr<CollisionPrimitive>>clonePrimitiveArray;
	for (auto& primitive : m_primitiveArray)
	{
		clonePrimitiveArray.emplace_back(std::shared_ptr<CollisionPrimitive>(primitive->Clone(arg_parent)));
	}

	return Collider(
		m_name + " - Clone",
		clonePrimitiveArray,
		m_callBack
	);
}

bool Collider::CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter)
{
	auto other = Other.lock();

	//いずれかのコライダーが有効でない
	if (!this->m_isActive || !other->m_isActive)return false;

	//判定
	Vec3<float>inter;
	bool hit = false;

	while (!hit)
	{
		for (auto& primitiveA : this->m_primitiveArray)
		{
			for (auto& primitiveB : other->m_primitiveArray)
			{
				hit = Collision::CheckPrimitiveHit(primitiveA.get(), primitiveB.get(), &inter);
			}
		}
	}

	if (Inter)*Inter = inter;
	return hit;
}

void Collider::DebugDraw(Camera& Cam)
{
	if (!m_isActive)return;

	for (auto& primitive : m_primitiveArray)
	{
		primitive->DebugDraw(m_isHit, Cam);
	}
}
