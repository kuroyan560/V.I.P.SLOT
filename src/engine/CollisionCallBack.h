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
	/// �Փˎ��Ăяo�����
	/// </summary>
	/// <param name="arg_inter">�Փ˓_</param>
	/// <param name="arg_myCollider">�������̃R���C�_�[</param>
	/// <param name="arg_otherColider">���葤�̃R���C�_�[</param>
	virtual void OnCollision(const Vec3<float>& arg_inter,std::weak_ptr<Collider>arg_myCollider, const std::weak_ptr<Collider> arg_otherColider) = 0;
public:
	virtual ~CollisionCallBack() {}
	virtual CollisionCallBack* Clone()
	{
		//�ݒ�Ȃ��A�N���[�������o���Ȃ�
		assert(0);
		return nullptr;
	}
};