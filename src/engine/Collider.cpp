#include "Collider.h"
#include"Collision.h"

int Collider::s_id = 0;

Collider Collider::Clone(Transform* arg_parent, ColliderParentObject* arg_parentObj)
{
	//判定用プリミティブクローン
	std::vector<std::shared_ptr<CollisionPrimitive>>clonePrimitiveArray;
	for (auto& primitive : m_primitiveArray)
	{
		clonePrimitiveArray.emplace_back(std::shared_ptr<CollisionPrimitive>(primitive->Clone(arg_parent)));
	}

	//クローン生成
	Collider clone;
	clone.Generate(m_name + " - Clone", m_tag, clonePrimitiveArray, arg_parentObj);

	//コールバック関数をコピー
	clone.m_callBackList = this->m_callBackList;
	return clone;
}

void Collider::Generate(const std::string& arg_name,
	const std::string& arg_tag, 
	const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray, 
	ColliderParentObject* arg_parentObj)
{
	m_name = arg_name;
	m_tag = arg_tag;
	m_primitiveArray = arg_primitiveArray;
	m_parentObj = arg_parentObj;
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
			hit = primitiveA->HitCheckDispatch(primitiveB.get(), &inter);
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

void Collider::SetCallBack(std::string arg_otherTag, CollisionCallBack* arg_callBack)
{
	m_callBackList[arg_otherTag].emplace_back(arg_callBack);
}

void Collider::SetParentTransform(Transform* arg_parent)
{
	for (auto& p : m_primitiveArray)p->SetParentTransform(arg_parent);
}
