#pragma once
#include"CoinVault.h"
#include<memory>
#include"Transform.h"
class EnemyBreed;
class EnemyController;
class LightManager;
class Camera;

class Enemy
{
protected:
	Enemy() {}

	friend class EnemyController;

	//����
	std::weak_ptr<EnemyBreed>m_breed;
	//�g�����X�t�H�[��
	Transform m_transform;
	//������
	CoinVault m_coinVault;
	//HP
	int m_hp;
	//�R���g���[���[
	std::unique_ptr<EnemyController>m_controller;

public:
	//������
	void Init(const std::shared_ptr<EnemyBreed>& arg_breed);
	//�X�V
	void Update();
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	/// <summary>
	/// �_���[�W���󂯂�
	/// </summary>
	/// <param name="arg_amount">�_���[�W��</param>
	/// <returns>���S�����ꍇ�R�C�����A���S���ĂȂ���΂O��Ԃ�/returns>
	int Damage(int arg_amount = 1);
	//�ꌂ�Ŏ���
	int Blow() { return Damage(m_hp); }

	//��ʔԍ��Q�b�^
	const int& GetTypeID();

	//�����t���O
	bool IsDead();
};