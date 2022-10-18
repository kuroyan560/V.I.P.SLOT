#pragma once
#include"Vec.h"
#include<memory>
class Collider;

//�Փ˔��肪�������ۂɌĂяo�����
class CollisionCallBack
{
private:
	friend class CollisionManager;

protected:
	/// <summary>
	/// �Փ˒��Ăяo�����
	/// </summary>
	/// <param name="arg_inter">�Փ˓_</param>
	/// <param name="arg_otherCollider">���葤�̃R���C�_�[</param>
	virtual void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider) = 0;

	/// <summary>
	/// �Փ˂����u�ԌĂяo�����
	/// </summary>
	/// <param name="arg_inter">�Փ˓_</param>
	/// <param name="arg_otherCollider">���葤�̃R���C�_�[</param>
	virtual void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider) = 0;

public:
	virtual ~CollisionCallBack() {}
	virtual CollisionCallBack* Clone()
	{
		//�ݒ�Ȃ��A�N���[�������o���Ȃ�
		assert(0);
		return nullptr;
	}
};