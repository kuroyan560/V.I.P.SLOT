#pragma once
#include"Vec.h"
class CollisionPrimitive;
//�Փ˔���̌��ʏ��
class CollisionResultInfo
{
public:
	//�Փ˓_
	Vec3<float>m_inter;
	//�����蔻��̍���������̃v���~�e�B�u
	CollisionPrimitive* m_hitOtherPrimitive = nullptr;
};