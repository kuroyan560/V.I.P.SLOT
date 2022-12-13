#pragma once
#include"Vec.h"
#include<memory>
class Collider;
class CollisionResultInfo;

//�Փ˔��肪�������ۂɌĂяo�����
class CollisionCallBack
{
private:
	friend class CollisionManager;

protected:
	/// <summary>
	/// �Փ˒��Ăяo�����
	/// </summary>
	/// <param name="arg_info">�Փˌ��ʏ��</param>
	/// <param name="arg_myCollider">�������̃R���C�_�[</param>
	/// <param name="arg_otherCollider">���葤�̃R���C�_�[</param>
	virtual void OnCollisionEnter(
		const CollisionResultInfo& arg_info,
		std::weak_ptr<Collider>arg_myCollider,
		std::weak_ptr<Collider>arg_otherCollider) = 0;

	/// <summary>
	/// �Փ˂����u�ԌĂяo�����
	/// </summary>
	/// <param name="arg_info">�Փˌ��ʏ��</param>
	/// <param name="arg_myCollider">�������̃R���C�_�[</param>
	/// <param name="arg_otherCollider">���葤�̃R���C�_�[</param>
	virtual void OnCollisionTrigger(
		const CollisionResultInfo& arg_info,
		std::weak_ptr<Collider>arg_myCollider,
		std::weak_ptr<Collider>arg_otherCollider) = 0;

public:
	virtual ~CollisionCallBack() {}
	virtual CollisionCallBack* CallBackClone()
	{
		//�ݒ�Ȃ��A�N���[�������o���Ȃ�
		assert(0);
		return nullptr;
	}
};