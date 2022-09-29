#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
#include"Timer.h"
#include<vector>
class ModelObject;
class LightManager;
class Camera;
class SlotMachine;
class TimeScale;
class Collider;
class CollisionManager;

class Player
{
	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject>m_modelObj;

	//�ړ�
	Vec3<float>m_move = { 0,0,0 };

	//�������x
	float m_fallSpeed;

	//�ڒn�t���O
	bool m_isOnGround;

	//������
	CoinVault m_coinVault;

	//BET�̃X�p���v���p�^�C�}�[
	Timer m_betTimer;
	//�A��BET�̌v���p�^�C�}�[
	Timer m_consecutiveBetTimer;
	//BET��SE
	int m_betSE;

	//�R���C�_�[�z��
	std::vector<std::shared_ptr<Collider>>m_colliders;

public:
	Player(std::weak_ptr<CollisionManager>arg_collisionMgr);
	//������
	void Init();
	//�X�V
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, const TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	CoinVault& GetVault() { return m_coinVault; }
};