#pragma once
#include<memory>
#include<array>
#include<string>
#include"Reel.h"
class ModelObject;
class LightManager;
class Camera;

class SlotMachine
{
	//�X���b�g�}�V��
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//���[��
	enum REEL { LEFT, CENTER, RIGHT, NUM };
	const std::array<std::string, REEL::NUM>REEL_MATERIAL_NAME =
	{
		"Reel_Left","Reel_Center","Reel_Right"
	};
	std::array<Reel, REEL::NUM>m_reels;

	//�X���b�g�̃��o�[�t���O�iStart��Stop���j
	int m_lever;
	//�Ō�̃��[�����~�߂Ă���̎��Ԍv��
	int m_slotWaitTimer;

	//�T�E���h
	int m_spinStartSE;	//��]�X�^�[�g
	int m_reelStopSE;		//���[���X�g�b�v
	
public:
	SlotMachine();
	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};