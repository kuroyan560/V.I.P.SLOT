#pragma once
#include<memory>
#include"Vec.h"
#include"Angle.h"
class ObjectManager;
class CollisionManager;

//�e��ł@�\�C���^�[�t�F�[�X�i�݂��Ɉˑ����Ȃ��悤�����̖�����S���j
//Muzzle = �e��
class Muzzle
{
private:
	static std::weak_ptr<ObjectManager>s_objMgr;

protected:
	Muzzle() {}
	void Shot(std::weak_ptr<CollisionManager>arg_collisionMgr,
		Vec3<float>arg_startPos,
		Vec2<float>arg_moveDirXY,
		float arg_speed,
		bool arg_sinMeandeling = false,
		float arg_meandelingInterval = 60.0f,
		Angle arg_meandelingAngle = Angle(30));

public:
	~Muzzle() {}

	static void AttachObjectManager(std::weak_ptr<ObjectManager>arg_objMgr)
	{
		s_objMgr = arg_objMgr;
	}
};