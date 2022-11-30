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
#include"ConstParameters.h"
#include"Debugger.h"
class ModelObject;
class LightManager;
class GameCamera;
class TimeScale;
class Player;
class RenderTarget;

class SlotMachine : public Debugger
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

	/*--- �X���b�g�Q�[�W ---*/
	//�X���b�g�Q�[�W���f����ʃ����_�[�^�[�Q�b�g
	std::shared_ptr<RenderTarget>m_slotGaugeScreen;
	//�X���b�g�Q�[�W�摜
	std::shared_ptr<TextureBuffer>m_slotGaugeTex;
	//�X���b�g��]�\��
	int m_startSlotCount = 0;

	/*--- �������� ---*/
	//��������^�C�}�[
	std::array<Timer, ConstParameter::Slot::AUTO_OPERATE_NUM> m_autoTimer;
	//�X���b�g�Q�[�W��������^�C���X�P�[���i�Q�[�W�ʂ������قǎ��ԊԊu�����܂�j
	float m_autoOperateTimeScale = 1.0f;

	//�G�����m�F���đS�Ĉꏏ�Ȃ���ʔ���
	bool CheckReelPattern();
	//�X���b�g�̌��ʂ��牉�o��I��
	void SlotPerform(const ConstParameter::Slot::PATTERN& arg_pattern);
	
	//�S���[������~����
	bool IsStop()const { return m_lever == REEL::NONE; }

	//�X���b�g�Q�[�W��ʍX�V
	void UpdateSlotGaugeScreen();

	void OnImguiItems()override;

public:
	SlotMachine();
	//������
	void Init(	);
	//�X�V
	void Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//���o�[����i�G��������true��Ԃ��j
	bool Lever();

	//�X���b�g�\��
	void Booking();

	//���[���ݒ�
	void ReelSet(REEL arg_which, std::shared_ptr<TextureBuffer>& arg_reelTex, std::vector<ConstParameter::Slot::PATTERN>& arg_patternArray);
};