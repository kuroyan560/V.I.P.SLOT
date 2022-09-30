#include "Collider.h"
#include"Collision.h"

int Collider::s_id = 0;

Collider::Collider(const std::string& arg_name, 
	const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray)
	:m_id(s_id++), m_name(arg_name), m_primitiveArray(arg_primitiveArray)
{
}

Collider Collider::Clone(Transform* arg_parent)
{
	//判定用プリミティブクローン
	std::vector<std::shared_ptr<CollisionPrimitive>>clonePrimitiveArray;
	for (auto& primitive : m_primitiveArray)
	{
		clonePrimitiveArray.emplace_back(std::shared_ptr<CollisionPrimitive>(primitive->Clone(arg_parent)));
	}

	//クローン生成
	auto clone = Collider(m_name + " - Clone", clonePrimitiveArray);

	//コールバック関数をコピー
	clone.m_callBacks = this->m_callBacks;
	return clone;
}

bool Collider::CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter)
{
	auto other = Other.lock();

	//いずれかのコライダーが有効でない
	if (!this->m_isActive || !other->m_isActive)return false;

	//判定
	Vec3<float>inter;
	bool hit = false;

	for (auto& primitiveA : this->m_primitiveArray)
	{
		for (auto& primitiveB : other->m_primitiveArray)
		{
			hit = Collision::CheckPrimitiveHit(primitiveA.get(), primitiveB.get(), &inter);
			if (hit)break;
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

void Collider::SetCallBack(const std::shared_ptr<CollisionCallBack>& arg_callBack, unsigned char arg_otherAttribute)
{
	//指定された振る舞いに対するコールバックは定義済
	if (m_callBacks.find(arg_otherAttribute) != m_callBacks.end())
	{
		printf("This attribute has already has callback.\n");
		assert(0);
	}

	m_callBacks[arg_otherAttribute] = arg_callBack;
}
