#pragma once
#include"CoinVault.h"
#include<memory>
#include"Transform.h"
#include"ColliderParentObject.h"
#include"Timer.h"
class ObjectBreed;
class ObjectController;
class TimeScale;
class LightManager;
class Camera;
class Collider;

class GameObject : public ColliderParentObject
{
private:
	friend class ObjectManager;

	//�R���g���[���[
	std::unique_ptr<ObjectController>m_controller;

	//�R���C�_�[�z��
	std::vector<std::shared_ptr<Collider>>m_colliders;

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
	void Update(const TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	/// <summary>
	/// �_���[�W���󂯂�
	/// </summary>
	/// <param name="arg_amount">�_���[�W��</param>
	/// <returns>���S�����ꍇ�R�C�����A���S���ĂȂ���΂O��Ԃ�</returns>
	int Damage(int arg_amount = 1);

	//��ʔԍ��Q�b�^
	const int& GetTypeID();

	//�����t���O
	bool IsDead();
};