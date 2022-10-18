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

//���C�L���X�g�ɂ�铖���蔻��̏����i�[�����\����
struct RaycastHitInfo
{
	//�Փ˂����R���C�_�[
	std::weak_ptr<Collider>m_otherCol;
	//�Փ˓_
	Vec3<float>m_inter;
	//�Փ˓_�܂ł̋���
	float m_distToInter;
};

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

	//���C�𔭎˂��ēo�^����Ă���S�ẴR���C�_�[�Ɠ����蔻��
	bool RaycastHit(Vec3<float>arg_start, Vec3<float>arg_dir, RaycastHitInfo* arg_hitInfo, std::string arg_targetTag = "", float arg_maxDist = FLT_MAX);

	//�R���C�_�[�̓o�^
	void Register(const std::shared_ptr<Collider>& arg_collider);
	void Register(const std::vector<std::shared_ptr<Collider>>& arg_colliderArray)
	{
		for (auto& col : arg_colliderArray)Register(col);
	}
	//�R���C�_�[�̓o�^����
	void Remove(const std::shared_ptr<Collider>& arg_collider);
};

