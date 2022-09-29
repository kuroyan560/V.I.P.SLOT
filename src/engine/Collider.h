#pragma once
#include<memory>
#include"Collision.h"

class Collider
{
private:
	friend class CollisionManager;

	//�t�^���鎯�ʔԍ�
	static int s_id;

	//���ʔԍ�
	int m_id = -1;

	//�R���C�_�[��
	std::string m_name;

	//�Փ˔���p�v���~�e�B�u
	std::shared_ptr<CollisionPrimitive>m_primitive;

	//�L���t���O
	bool m_isActive = true;

	//�����蔻�肪���������t���O
	bool m_isHit = false;

public:
	Collider(const std::string& arg_name, const std::shared_ptr<CollisionPrimitive>& arg_primitive);
	//�N���[��
	Collider(const Collider& arg_origin, Transform* arg_parent = nullptr);

	~Collider()	{}

	//�����蔻��i�Փ˓_��Ԃ��j
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//�����蔻��`��
	void DebugDraw(Camera& Cam);

	//�Z�b�^
	void SetActive(const bool& Active) { m_isActive = Active; }

	//�Q�b�^
	const bool& GetIsHit()const { return m_isHit; }
	const std::weak_ptr<CollisionPrimitive>GetColliderPrimitive()const { return m_primitive; }
};