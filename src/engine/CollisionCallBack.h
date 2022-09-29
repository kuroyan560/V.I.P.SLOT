#pragma once
#include"Vec.h"
#include<memory>
class Collider;
class CollisionManager;

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
	/// <param name="arg_otherAttribute">���葤�̃R���C�_�[�U�镑��</param>
	/// <param name="arg_collisionMgr">�R���C�_�[�}�l�[�W���i�U�镑���p�����[�^�擾�̂��߁j</param>
	virtual void OnCollision(
		const Vec3<float>& arg_inter,
		unsigned char& arg_otherAttribute, 
		const CollisionManager& arg_collisionMgr) = 0;
public:
	virtual ~CollisionCallBack() {}
	virtual CollisionCallBack* Clone()
	{
		//�ݒ�Ȃ��A�N���[�������o���Ȃ�
		assert(0);
		return nullptr;
	}
};