#pragma once
#include"Vec.h"
#include<memory>
class Collider;
class CollisionManager;

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
	/// <param name="arg_otherAttribute">相手側のコライダー振る舞い</param>
	/// <param name="arg_collisionMgr">コライダーマネージャ（振る舞いパラメータ取得のため）</param>
	virtual void OnCollision(
		const Vec3<float>& arg_inter,
		unsigned char& arg_otherAttribute, 
		const CollisionManager& arg_collisionMgr) = 0;
public:
	virtual ~CollisionCallBack() {}
	virtual CollisionCallBack* Clone()
	{
		//設定なし、クローン生成出来ない
		assert(0);
		return nullptr;
	}
};