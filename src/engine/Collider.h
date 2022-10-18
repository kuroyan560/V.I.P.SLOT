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

	//�L���t���O
	bool m_isActive = true;

	//�ߋ��̓����蔻��t���O�L�^
	bool m_oldIsHit = false;
	//�����蔻�肪���������t���O
	bool m_isHit = false;

	//�^�O
	std::string m_tag;
	//�R�[���o�b�N���X�g�A�L�[�͑���̃^�O��
	std::map<std::string, std::vector<CollisionCallBack*>>m_callBackList;

public:
	Collider() :m_id(s_id++) {}

	//�g�����X�t�H�[�������ς��ăN���[������
	Collider Clone(Transform* arg_parent, ColliderParentObject* arg_parentObj = nullptr);

	~Collider()	{}

	/// <summary>
	/// �R���C�_�[������
	/// </summary>
	/// <param name="arg_name">�R���C�_�[��</param>
	/// <param name="arg_tag">�R���C�_�[�^�O��</param>
	/// <param name="arg_primitiveArray">�R���C�_�[�ɃA�^�b�`�����v���~�e�B�u�z��</param>
	/// <param name="arg_parentObj">�R���C�_�[�̐e�ƂȂ�I�u�W�F�N�g</param>
	void Generate(const std::string& arg_name,
		const std::string& arg_tag,
		const std::vector<std::shared_ptr<CollisionPrimitive>>& arg_primitiveArray,
		ColliderParentObject* arg_parentObj = nullptr);

	//�����蔻��i�Փ˓_��Ԃ��j
	bool CheckHitCollision(std::weak_ptr<Collider> arg_other, Vec3<float>* arg_inter = nullptr);

	//�����蔻��`��
	void DebugDraw(Camera& arg_cam);

	//�Z�b�^
	void SetActive(const bool& arg_active) { m_isActive = arg_active; }
	/// <summary>
	/// �R�[���o�b�N�̃Z�b�g
	/// </summary>
	/// <param name="arg_otherTag">�R�[���o�b�N���Ăяo�������ƂȂ鑊��̃^�O��</param>
	/// <param name="arg_callBack">�Z�b�g����R�[���o�b�N</param>
	void SetCallBack(std::string arg_otherTag, CollisionCallBack* arg_callBack);
	void SetParentTransform(Transform* arg_parent);

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