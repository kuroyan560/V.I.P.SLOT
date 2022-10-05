#pragma once
#include"CoinVault.h"
#include<memory>
#include"Transform.h"
#include"ColliderParentObject.h"
#include"Timer.h"
class EnemyBreed;
class EnemyController;
class TimeScale;
class LightManager;
class Camera;
class Collider;

class Enemy : public ColliderParentObject
{
private:
	friend class EnemyManager;

	//������
	CoinVault m_coinVault;
	//HP
	int m_hp;
	//�R���g���[���[
	std::unique_ptr<EnemyController>m_controller;

	//�R���C�_�[�z��
	std::vector<std::shared_ptr<Collider>>m_colliders;

	//��_���[�W���̖��G����
	Timer m_damagedInvincibleTimer;

	//��_���[�W���̉��~�I�t�Z�b�gY
	Timer m_damagedOffsetTimer;
	float m_oldDamagedOffsetY;
	float m_damagedOffsetY;

public:
	//�g�����X�t�H�[��
	Transform m_transform;
	//����
	std::weak_ptr<EnemyBreed>m_breed;

	//����������ɐ���
	Enemy(const std::shared_ptr<EnemyBreed>& arg_breed);

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
	//�ꌂ�Ŏ���
	int Blow() { return Damage(m_hp); }

	//��ʔԍ��Q�b�^
	const int& GetTypeID();

	//�����t���O
	bool IsDead();
	//�_���[�W�Ŏ��񂾂�
	bool IsKilled();
	//���G��Ԃ�
	bool IsInvincible()
	{
		return !m_damagedInvincibleTimer.IsTimeUp();
	}
};