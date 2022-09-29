#pragma once
#include<memory>
#include"Collision.h"
#include<vector>
class CollisionCallBack;

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

	//�Փ˔���p�v���~�e�B�u�z��
	std::vector<std::shared_ptr<CollisionPrimitive>>m_primitiveArray;

	//�R�[���o�b�N�֐�
	std::shared_ptr<CollisionCallBack>m_callBack;

	//�L���t���O
	bool m_isActive = true;

	//�����蔻�肪���������t���O
	bool m_isHit = false;

public:
	Collider(const std::string& arg_name,
		const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray,
		const std::shared_ptr<CollisionCallBack>& arg_callBack);

	~Collider()	{}

	//�����蔻��i�Փ˓_��Ԃ��j
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//�����蔻��`��
	void DebugDraw(Camera& Cam);

	//�Z�b�^
	void SetActive(const bool& Active) { m_isActive = Active; }

	//�Q�b�^
	const bool& GetIsHit()const { return m_isHit; }
};