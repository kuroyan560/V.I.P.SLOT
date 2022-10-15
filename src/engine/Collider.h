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
class ColliderParentObject;

class Collider
{
private:
	friend class CollisionManager;

	//�t�^���鎯�ʔԍ�
	static int s_id;

	//���g���A�^�b�`�����e�I�u�W�F�N�g
	ColliderParentObject* m_parentObj = nullptr;

	//���ʔԍ�
	int m_id = -1;

	//�R���C�_�[��
	std::string m_name;

	//�Փ˔���p�v���~�e�B�u�z��
	std::vector<std::shared_ptr<CollisionPrimitive>>m_primitiveArray;

	//�R�[���o�b�N�֐��i����̐U�镑���ɂ���ČĂяo���R�[���o�b�N��ς�����j
	std::map<unsigned char, CollisionCallBack*>m_callBacks;

	//�L���t���O
	bool m_isActive = true;

	//�ߋ��̓����蔻��t���O�L�^
	bool m_oldIsHit = false;
	//�����蔻�肪���������t���O
	bool m_isHit = false;

public:
	Collider(const std::string& arg_name,
		const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray,
		ColliderParentObject* arg_parentObj = nullptr);

	//�g�����X�t�H�[�������ς��ăN���[������
	Collider Clone(Transform* arg_parent, ColliderParentObject* arg_parentObj = nullptr);

	~Collider()	{}

	//�����蔻��i�Փ˓_��Ԃ��j
	bool CheckHitCollision(std::weak_ptr<Collider> Other, Vec3<float>* Inter = nullptr);

	//�����蔻��`��
	void DebugDraw(Camera& Cam);

	//�Z�b�^
	void SetActive(const bool& Active) { m_isActive = Active; }
	void SetCallBack(CollisionCallBack* arg_callBack, unsigned char arg_otherAttribute = UCHAR_MAX);
	void SetParentTransform(Transform* arg_parent);
	void SetParentObj(ColliderParentObject* arg_parentObj)
	{
		m_parentObj = arg_parentObj;
	}

	//�Q�b�^
	const bool& GetIsHit()const { return m_isHit; }
	//�R���C�_�[���A�^�b�`����Ă�e�I�u�W�F�N�g�擾
	template<typename T>
	T* GetParentObject()
	{
		//�A�^�b�`����Ă��Ȃ�
		if (m_parentObj == nullptr)
		{
			printf("This collider's parent object hasn't be attatched. It's nullptr.\n");
			assert(0);
		}

		//�w�肳�ꂽ�^�ł͂Ȃ�
		if (typeid(T) != typeid(*m_parentObj))
		{
			printf("This collider's parent object isn't \" %s \" class.\n", typeid(T).name());
			assert(0);
		}
		return (T*)m_parentObj;
	}

	bool operator== (const Collider& arg_other)const
	{
		return this->m_id == arg_other.m_id;
	}
};