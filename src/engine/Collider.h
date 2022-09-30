#pragma once
#include<memory>
#include<vector>
#include<string>
#include"Vec.h"
#include"Transform.h"
#include<map>
class Camera;
class CollisionPrimitive;
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

	//�R�[���o�b�N�֐��i����̐U�镑���ɂ���ČĂяo���R�[���o�b�N��ς�����j
	std::map<unsigned char, std::shared_ptr<CollisionCallBack>>m_callBacks;

	//�L���t���O
	bool m_isActive = true;

	//�����蔻�肪���������t���O
	bool m_isHit = false;

public:
	Collider(const std::string& arg_name,
		const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray);

	//�g�����X�t�H�[�������ς��ăN���[������
	Collider Clone(Transform* arg_parent);

	~Collider()	{}

	//�����蔻��i�Փ˓_��Ԃ��j
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//�����蔻��`��
	void DebugDraw(Camera& Cam);

	//�Z�b�^
	void SetActive(const bool& Active) { m_isActive = Active; }
	void SetCallBack(const std::shared_ptr<CollisionCallBack>& arg_callBack, unsigned char arg_otherAttribute = UCHAR_MAX);

	//�Q�b�^
	const bool& GetIsHit()const { return m_isHit; }

	bool operator== (const Collider& arg_other)const
	{
		return this->m_id == arg_other.m_id;
	}
};