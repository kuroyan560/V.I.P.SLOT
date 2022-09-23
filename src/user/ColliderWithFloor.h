#pragma once
#include"Collider.h"
#include<memory>
#include"Transform.h"
class Player;
class Collider;

class ColliderWithFloor
{
	//���Ƃ̉����߂��R�[���o�b�N
	class CallBack : public CollisionCallBack
	{
		ColliderWithFloor* m_parent;
		void OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)override;
	public:
		CallBack(ColliderWithFloor* Parent) :m_parent(Parent) {}
	}m_callBack;

	//�����߂������̑Ώۂ�Transform
	Transform* m_targetTransform;

	//���Ɠ����蔻�肷��p�̃R���C�_�[
	std::shared_ptr<Collider>m_collider;

	//�ڒn�t���O
	bool m_onGround = false;

	//�������x
	float m_fallSpeed = 0.0f;

public:
	ColliderWithFloor(const std::string& ParentName, const Vec3<float>& Offset, Transform* ParentTransform);
	//������
	void Init()
	{
		//m_onGround = false;
		m_onGround = true;
		m_fallSpeed = 0.0f;
	}
	//�W�����v
	void Jump(const float& JumpPower)
	{
		m_onGround = false;
		m_fallSpeed = JumpPower;
	}
	//�������x����
	void AddFallSpeed(const float& Gravity)
	{
		m_fallSpeed += Gravity;
	}
	//�������x���Z�b�g
	void ResetFallSpeed()
	{
		m_fallSpeed = 0.0f;
	}

	//�Q�b�^
	const bool& GetOnGround()const { return m_onGround; }
	const float& GetFallSpeed()const { return m_fallSpeed; }
	const std::shared_ptr<Collider>GetCollider() { return m_collider; }
};