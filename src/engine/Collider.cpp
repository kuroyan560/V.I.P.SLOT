#include "Collider.h"

int Collider::s_id = 0;

Collider::Collider(const std::string& arg_name, const std::shared_ptr<CollisionPrimitive>& arg_primitive)
	:m_id(s_id++), m_name(arg_name), m_primitive(arg_primitive)
{
}

Collider::Collider(const Collider& arg_origin, Transform* arg_transform)
	:Collider(arg_origin.m_name + " - Clone",
		std::shared_ptr<CollisionPrimitive>(arg_origin.GetColliderPrimitive().lock()->Clone(arg_transform)))
{
}

bool Collider::CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter)
{
	auto other = Other.lock();

	//いずれかのコライダーが有効でない
	if (!this->m_isActive || !other->m_isActive)return false;

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
