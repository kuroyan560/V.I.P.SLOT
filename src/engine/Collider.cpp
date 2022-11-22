#include "Collider.h"
#include"Collision.h"

int Collider::s_id = 0;

Collider Collider::Clone(Transform* arg_parent, ColliderParentObject* arg_parentObj)
{
	//����p�v���~�e�B�u�N���[��
	std::vector<std::shared_ptr<CollisionPrimitive>>clonePrimitiveArray;
	for (auto& primitive : m_primitiveArray)
	{
		clonePrimitiveArray.emplace_back(std::shared_ptr<CollisionPrimitive>(primitive->Clone()));
	}

	//�N���[������
	Collider clone;
	clone.Generate(m_name + " - Clone", m_tags, clonePrimitiveArray);
	clone.SetParentTransform(arg_parent);
	clone.SetParentObject(arg_parentObj);

	//�R�[���o�b�N�֐����R�s�[
	clone.m_callBackList = this->m_callBackList;
	return clone;
}

void Collider::Generate(const std::string& arg_name,
	const std::vector<std::string>& arg_tags, 
	const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray)
{
	m_name = arg_name;
	m_tags = arg_tags;
	m_primitiveArray = arg_primitiveArray;
}

bool Collider::CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter)
{
	auto other = Other.lock();

	//�����ꂩ�̃R���C�_�[���L���łȂ�
	if (!this->m_isActive || !other->m_isActive)return false;

	//����
	Vec3<float>inter;
	bool hit = false;

	for (auto& primitiveA : this->m_primitiveArray)
	{
		for (auto& primitiveB : other->m_primitiveArray)
		{
			hit = primitiveA->HitCheckDispatch(
				this->GetTransformMat(),
				other->GetTransformMat(),
				primitiveB.get(),
				&inter);
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
		primitive->DebugDraw(m_isHit, Cam,
			this->GetTransformMat(),
			this->GetDepth());
	}
}

void Collider::SetCallBack(std::string arg_otherTag, CollisionCallBack* arg_callBack)
{
	m_callBackList[arg_otherTag].emplace_back(arg_callBack);
}

void Collider::SetParentTransform(Transform* arg_parent)
{
	m_parentTransform = arg_parent;
}

void Collider::SetParentObject(ColliderParentObject* arg_parent)
{
	m_parentObj = arg_parent;
}

bool Collider::HaveCallBack(std::weak_ptr<Collider> arg_other)
{
	//����R���C�_�[�ɕt�^����Ă���^�O�𑖍�
	for (auto& tag : arg_other.lock()->m_tags)
	{
		//�����ɓn�����R���C�_�[���Ώۂ̃R�[���o�b�N����������true��Ԃ�
		if (m_callBackList.find(tag) != m_callBackList.end())return true;
	}
	return false;
}
