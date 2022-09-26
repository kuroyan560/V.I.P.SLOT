#pragma once
#include<memory>
#include<array>
#include<string>
#include"Reel.h"
#include"CoinVault.h"
#include"Transform.h"
#include<forward_list>
class ModelObject;
class Model;
class LightManager;
class Camera;

class SlotMachine
{
	//�X���b�g�}�V��
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//�R�C�����������K�z��
	std::shared_ptr<ModelObject>m_megaPhoneObj;

	//�R�C�����f��
	std::shared_ptr<Model>m_coinModel;

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

	//������
	CoinVault m_coinVault;

	//BET���ꂽ�R�C���̏��
	struct BetCoin
	{
		//����̏�����
		CoinVault* m_otherVault;
		//�R�C����
		int m_coinNum;
		//�������݈ʒu
		Transform m_emitTransform;
		//���g�̃g�����X�t�H�[��
		Transform m_transform;
		//�������܂�Ă���̎���
		int m_timer = 0;
		//BET�ɐ���
		bool m_bet = false;

		BetCoin(CoinVault* arg_otherVault, int arg_coinNum, const Transform& arg_emitTransform)
			:m_otherVault(arg_otherVault), m_coinNum(arg_coinNum), m_emitTransform(arg_emitTransform), m_transform(arg_emitTransform) {}
	};
	std::forward_list<BetCoin>m_betCoinArray;
	
public:
	SlotMachine();
	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//BET��t
	void Bet(CoinVault& arg_otherVault, int arg_coinNum, const Transform& arg_emitTransform);
};