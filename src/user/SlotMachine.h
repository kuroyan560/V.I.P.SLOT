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
public:
	enum REEL { LEFT, CENTER, RIGHT, NUM, NONE = -1 };

private:
	//�X���b�g�}�V��
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//�R�C�����������K�z��
	std::shared_ptr<ModelObject>m_megaPhoneObj;

	//���[��
	const std::array<std::string, REEL::NUM>REEL_MATERIAL_NAME =
	{
		"Reel_Left","Reel_Center","Reel_Right"
	};
	std::array<Reel, REEL::NUM>m_reels;

	//�X���b�g�̃��o�[�t���O�iStart��Stop���j
	int m_lever;

	//�T�E���h
	int m_spinStartSE;	//��]�X�^�[�g
	int m_reelStopSE;		//���[���X�g�b�v

	//�G���̒�`�N���X
	PatternManager m_patternMgr;

	//�X���b�g��]�\��
	int m_startSlotCount = 0;

	enum AUTO_OPERATE
	{
		UNTIL_FIRST_REEL,	//�ŏ��̃��[����~�܂ł̎���
		REEL_STOP_SPAN,	//���[�����Ƃ̒�~�܂ł̎���
		AFTER_STOP_ALL_REEL,	//�S���[����~��̑҂�����
		AUTO_OPERATE_NUM
	};
	//��������^�C�}�[
	std::array<Timer, AUTO_OPERATE_NUM> m_autoTimer;

	//�G�����m�F���đS�Ĉꏏ�Ȃ���ʔ���
	bool CheckReelPattern();
	//�X���b�g�̌��ʂ��牉�o��I��
	void SlotPerform(const ConstParameter::Slot::PATTERN& arg_pattern);
	
	//�S���[������~����
	bool IsStop()const { return m_lever == REEL::NONE; }

public:
	SlotMachine();
	//������
	void Init(	);
	//�X�V
	void Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam);
	//imgui�f�o�b�O
	void ImguiDebug();

	//���o�[����i�G��������true��Ԃ��j
	bool Lever();

	//�X���b�g�\��
	void Booking();

	//���[���ݒ�
	void ReelSet(REEL arg_which, std::shared_ptr<TextureBuffer>& arg_reelTex, std::vector<ConstParameter::Slot::PATTERN>& arg_patternArray);
};