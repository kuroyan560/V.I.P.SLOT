#pragma once
#include"Vec.h"
#include<memory>
class Collider;

//衝突判定があった際に呼び出される
class CollisionCallBack
{
private:
	friend class CollisionManager;

protected:
	/// <summary>
	/// 衝突時呼び出される
	/// </summary>
	/// <param name="arg_inter">衝突点</param>
	/// <param name="arg_myCollider">自分側のコライダー</param>
	/// <param name="arg_otherColider">相手側のコライダー</param>
	virtual void OnCollision(const Vec3<float>& arg_inter,std::weak_ptr<Collider>arg_myCollider, const std::weak_ptr<Collider> arg_otherColider) = 0;
public:
	virtual ~CollisionCallBack() {}
	virtual CollisionCallBack* Clone()
	{
		//設定なし、クローン生成出来ない
		assert(0);
		return nullptr;
	}
};