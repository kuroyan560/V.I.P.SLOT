#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
class ModelObject;
class LightManager;
class Camera;

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
	int m_betTimer;
	//�A��BET�̌v���p�^�C�}�[
	int m_consecutiveBetTimer;
	//BET��SE
	int m_betSE;

public:
	Player();
	//������
	void Init();
	//�X�V
	void Update(CoinVault& arg_slotCoinVault);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);
};