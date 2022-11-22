#include "Muzzle.h"
#include"ObjectManager.h"
std::weak_ptr<ObjectManager>Muzzle::s_objMgr;

void Muzzle::Shot(std::weak_ptr<CollisionManager> arg_collisionMgr, Vec3<float> arg_startPos, Vec2<float> arg_moveDirXY, float arg_speed, bool arg_sinMeandeling, float arg_meandelingInterval, Angle arg_meandelingAngle)
{
	s_objMgr.lock()->AppearEnemyBullet(
		arg_collisionMgr,
		arg_startPos,
		arg_moveDirXY,
		arg_speed,
		arg_sinMeandeling,
		arg_meandelingInterval,
		arg_meandelingAngle);
}
