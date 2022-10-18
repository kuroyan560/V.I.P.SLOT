#pragma once
#include<map>
#include<string>
#include<list>
#include<memory>
#include<limits>
#include<vector>
#include"Vec.h"
class Collider;
class Camera;

class CollisionManager
{
	//登録されたコライダーリスト
	std::list<std::shared_ptr<Collider>>m_colliderList;

	//衝突していた場合の処理
	void OnHit(const std::shared_ptr<Collider>& arg_myCollider, const std::shared_ptr<Collider>& arg_otherCollider, const Vec3<float>& arg_inter);

public:
	CollisionManager() {}

	void Update();
	void DebugDraw(Camera& Cam);

	//コライダーの登録
	void Register(const std::shared_ptr<Collider>& arg_collider);
	void Register(const std::vector<std::shared_ptr<Collider>>& arg_colliderArray)
	{
		for (auto& col : arg_colliderArray)Register(col);
	}
	//コライダーの登録解除
	void Remove(const std::shared_ptr<Collider>& arg_collider);
};

