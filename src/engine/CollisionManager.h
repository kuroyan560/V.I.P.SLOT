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
	//�o�^���ꂽ�R���C�_�[���X�g
	std::list<std::shared_ptr<Collider>>m_colliderList;

	//�Փ˂��Ă����ꍇ�̏���
	void OnHit(const std::shared_ptr<Collider>& arg_myCollider, const std::shared_ptr<Collider>& arg_otherCollider, const Vec3<float>& arg_inter);

public:
	CollisionManager() {}

	void Update();
	void DebugDraw(Camera& Cam);

	//�R���C�_�[�̓o�^
	void Register(const std::shared_ptr<Collider>& arg_collider);
	void Register(const std::vector<std::shared_ptr<Collider>>& arg_colliderArray)
	{
		for (auto& col : arg_colliderArray)Register(col);
	}
	//�R���C�_�[�̓o�^����
	void Remove(const std::shared_ptr<Collider>& arg_collider);
};

