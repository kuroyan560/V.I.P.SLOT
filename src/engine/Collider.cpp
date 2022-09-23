#include "Collider.h"

int Collider::s_id = 0;
std::list<std::weak_ptr<Collider>>Collider::s_colliderList;

std::shared_ptr<Collider> Collider::Generate(
	const std::string& Name,
	const std::shared_ptr<CollisionPrimitive>& Primitive,
	CollisionCallBack* CallBack,
	const char& MyAttribute,
	const char& HitCheckAttrubute)
{
	auto instance = std::shared_ptr<Collider>(new Collider(Name, Primitive, CallBack, MyAttribute, HitCheckAttrubute));

	//コールバック関数にコライダーのweak_ptrをアタッチ
	if (CallBack)
	{
		instance->m_callBack->m_attachCollider = instance;
	}

	s_colliderList.emplace_back(instance);
	return instance;
}

std::shared_ptr<Collider>Collider::Clone(const std::shared_ptr<Collider>& Origin, Transform* Parent)
{
	auto clonePrimitive = std::shared_ptr<CollisionPrimitive>(Origin->GetColliderPrimitive().lock()->Clone(Parent));
	return Collider::Generate(
		Origin->m_name + " - Clone",
		clonePrimitive,
		Origin->m_callBack ? Origin->m_callBack->Clone() : nullptr,
		Origin->m_myAttribute,
		Origin->m_hitCheckAttribute);
}

void Collider::UpdateAllColliders()
{
	//既に寿命切れのコライダーを削除
	s_colliderList.remove_if([](std::weak_ptr<Collider>& col) {return col.expired(); });

	//当たり判定記録リセット
	for (auto& col : s_colliderList)col.lock()->m_isHit = false;

	//総当り衝突判定
	for (auto itrA = s_colliderList.begin(); itrA != s_colliderList.end(); ++itrA)
	{
		auto colA = itrA->lock();

		auto itrB = itrA;
		++itrB;
		for (; itrB != s_colliderList.end(); ++itrB)
		{
			auto colB = itrB->lock();
			Vec3<float>inter;
			if (colA->CheckHitCollision(colB, &inter))
			{
				colA->m_isHit = true;
				colB->m_isHit = true;
				//コールバック関数呼び出し
				if (colA->m_callBack && colB->CompareAttribute(colA->m_hitCheckAttribute))colA->m_callBack->OnCollision(inter, colB);
				if (colB->m_callBack && colA->CompareAttribute(colB->m_hitCheckAttribute))colB->m_callBack->OnCollision(inter, colA);
			}
		}
	}
}

void Collider::DebugDrawAllColliders(Camera& Cam)
{
	for (auto& col : s_colliderList)
	{
		if (!col.lock()->m_isActive)continue;
		col.lock()->DebugDraw(Cam);
	}
}

bool Collider::CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter)
{
	auto other = Other.lock();

	//いずれかのコライダーが有効でない
	if (!this->m_isActive || !other->m_isActive)return false;

	//お互いに衝突判定を行う相手ではない
	if (!(this->m_hitCheckAttribute & other->m_myAttribute) && !(this->m_myAttribute & other->m_hitCheckAttribute))return false;

	//判定
	Vec3<float>inter;
	bool hit = Collision::CheckPrimitiveHit(this->m_primitive.get(), other->m_primitive.get(),&inter);
	if (Inter)*Inter = inter;
	return hit;
}

void Collider::DebugDraw(Camera& Cam)
{
	if (!m_isActive)return;
	m_primitive->DebugDraw(m_isHit, Cam);
}
