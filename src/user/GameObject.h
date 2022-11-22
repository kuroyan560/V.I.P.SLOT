#pragma once
#include"CoinVault.h"
#include<memory>
#include"Transform.h"
#include"ColliderParentObject.h"
#include"Timer.h"
#include"CoinVault.h"
class ObjectBreed;
class ObjectController;
class TimeScale;
class LightManager;
class Camera;
class Collider;
class CollisionManager;

class GameObject : public ColliderParentObject
{
private:
	friend class ObjectManager;
	friend class ObjectController;

	//�R���g���[���[
	std::unique_ptr<ObjectController>m_controller;

	//�R���C�_�[�z��
	std::vector<std::shared_ptr<Collider>>m_colliders;

	//HP
	int m_hp;

	//�e�i���g�����X�t�H�[���̐e�ł͂Ȃ��j
	GameObject* m_parentObj;

public:
	//�g�����X�t�H�[��
	Transform m_transform;
	//����
	std::weak_ptr<ObjectBreed>m_breed;

	//����������ɐ���
	GameObject(const std::shared_ptr<ObjectBreed>& arg_breed);

	//������
	void Init();
	//�X�V
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	/// <summary>
	/// �_���[�W���󂯂�
	/// </summary>
	/// <param name="arg_amount">�_���[�W��</param>
	/// <returns>���S�����ꍇ�R�C�����A���S���ĂȂ���΂O��Ԃ�</returns>
	int Damage(int arg_amount = 1);

	//�e�I�u�W�F�N�g�Z�b�g
	void SetParentObj(GameObject* arg_parentObj) { m_parentObj = arg_parentObj; }

	//��ʔԍ��Q�b�^
	const int& GetTypeID();
	//�e�Q�[���I�u�W�F�N�g�擾
	GameObject* GetParentObj() { return m_parentObj; }

	//�����t���O
	bool IsDead();
};