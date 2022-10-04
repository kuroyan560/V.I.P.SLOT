#pragma once
#include<memory>
#include<array>
#include<string>
#include"Reel.h"
#include"CoinVault.h"
#include"Transform.h"
#include"Timer.h"
#include"PatternManager.h"
#include"CoinObjectManager.h"
#include"CoinPerform.h"
#include"ReturnCoinEmitter.h"
class ModelObject;
class LightManager;
class GameCamera;
class TimeScale;
class Player;

class SlotMachine
{
	//�X���b�g�}�V��
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//�R�C�����������K�z��
	std::shared_ptr<ModelObject>m_megaPhoneObj;

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
	Timer m_slotWaitTimer;

	//�T�E���h
	int m_spinStartSE;	//��]�X�^�[�g
	int m_reelStopSE;		//���[���X�g�b�v

	//������
	CoinVault m_coinVault;

	//BET���ꂽ�R�C���̋���
	CoinObjectManager m_betCoinObjManager;
	class BetCoinPerform : public CoinPerform
	{
		//��������
		Timer m_timer;
	public:
		BetCoinPerform(int arg_lifeTime)
		{
			m_timer.Reset(arg_lifeTime);
		}
		void OnUpdate(Coins& arg_coin, float arg_timeScale)override;
		void OnEmit(Coins& arg_coin)override {};
		bool IsDead(Coins& arg_coin)override { return m_timer.IsTimeUp(); }
	};

	//�ԋp�R�C���G�~�b�^�[
	ReturnCoinEmitter m_returnCoinEmitter;

	//BET���̃��K�z���g�k���o
	Timer m_megaPhoneExpandTimer;

	//�G���̒�`�N���X
	PatternManager m_patternMgr;

	//�G�����m�F���đS�Ĉꏏ�Ȃ���ʔ���
	bool CheckReelPattern();
	//�X���b�g�̌��ʂ��牉�o��I��
	void SlotPerform(const ConstParameter::Slot::PATTERN& arg_pattern, std::weak_ptr<Player>& arg_player);
	
public:
	SlotMachine();
	//������
	void Init();
	//�X�V
	void Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam);

	//BET��t
	void Bet(int arg_coinNum, const Transform& arg_emitTransform);
};