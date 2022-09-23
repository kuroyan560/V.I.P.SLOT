#pragma once
#include"ControllerConfig.h"
#include<array>
class UsersInput;

//�v���C���[�̏��
enum struct PLAYER_STATUS_TAG
{
	WAIT,					//�ҋ@
	MOVE,					//�ʏ�ړ�
	ATTACK,	//�ʏ�U��
	JUMP,					//�W�����v
	GUARD,					//�K�[�h
	DODGE,					//���
	RUN,					//�_�b�V��
	MARKING,				//�}�[�L���O
	CHARGE,				//�i�P�����b�V���j�̃`���[�W��
	RUSH,					//���b�V��
	INVOKE_ABILITY,		//�A�r���e�B����
	OUT_OF_CONTROL,	//������i������󂯕t���Ȃ��j
	NUM
};

//��ԑJ�ڂɕK�v�ȏ��̈ꎞ�i�[�p�\����
struct PlayerParameterForStatus
{
	int m_markingNum = 0;
	bool m_maxMarking = false;
	bool m_onGround = false;
	bool m_attackFinish = true;
	bool m_dodgeFinish = true;
	bool m_rushFinish = true;
	bool m_abilityFinish = true;
};

//�v���C���[�̏�ԑJ�ڊǗ�
class PlayerStatus
{
	PLAYER_STATUS_TAG m_status = PLAYER_STATUS_TAG::OUT_OF_CONTROL;
	PLAYER_STATUS_TAG m_oldStatus = m_status;	//�P�t���[���O�̏��

	//���̓t���[���̋L�^
	std::array<int, static_cast<int>(HANDLE_INPUT_TAG::NUM)>m_inputFrame;
	int m_leftStickInputFrame;

	//��x�{�^���𗣂��Ă��邩�̃t���O�i�������ł̘A���h�~�j
	std::array<bool, static_cast<int>(HANDLE_INPUT_TAG::NUM)>m_off;

	//�e��Ԓ��̑J��
	PLAYER_STATUS_TAG WaitUpdate(const PlayerParameterForStatus& Parameters);
	PLAYER_STATUS_TAG MoveUpdate(const PlayerParameterForStatus& Parameters);
	PLAYER_STATUS_TAG JumpUpdate(const PlayerParameterForStatus& Parameters);

	//�W�����v�K�[�h�t���O
	bool m_jumpGuard;

public:
	//������
	void Init(PLAYER_STATUS_TAG InitTag = PLAYER_STATUS_TAG::OUT_OF_CONTROL)
	{
		m_status = InitTag;
		m_oldStatus = m_status;

		//���̓t���[���̋L�^���Z�b�g
		m_inputFrame.fill(0);
		m_leftStickInputFrame = 0;

		//�{�^�������t���O���Z�b�g
		m_off.fill(true);

		//�W�����v�K�[�h�t���O���Z�b�g
		m_jumpGuard = false;
	}

	//��ԑJ�ڍX�V
	void Update(const UsersInput& Input, const ControllerConfig& Controller, const PlayerParameterForStatus& Parameters);

	//����s�\
	void SetOutOfControl() 
	{
		m_oldStatus = m_status;
		m_status = PLAYER_STATUS_TAG::OUT_OF_CONTROL; 
	}
	//����s�\����
	void QuitOutOfControl(PLAYER_STATUS_TAG InitTag = PLAYER_STATUS_TAG::WAIT)
	{
		Init(InitTag);
	}

	//���݂̏�Ԃ������ɓn���ꂽ���̂�
	bool CompareNowStatus(PLAYER_STATUS_TAG Tag) { return m_status == Tag; }
	bool CompareOldStatus(PLAYER_STATUS_TAG Tag) { return m_oldStatus == Tag; }

	//�X�e�[�^�X���ω������u��
	bool StatusTrigger()
	{
		return m_status != m_oldStatus;
	}
	//�����ɓn�����X�e�[�^�X�ɕω������u�Ԃ�
	bool StatusTrigger(PLAYER_STATUS_TAG Status, const bool& Old = false)
	{
		if (Old)return !CompareNowStatus(Status) && CompareOldStatus(Status);
		return CompareNowStatus(Status) && !CompareOldStatus(Status);
	}

	//�X�e�[�^�X�̃Q�b�^
	PLAYER_STATUS_TAG& GetNowStatus() { return m_status; }
	PLAYER_STATUS_TAG& GetOldStatus() { return m_oldStatus; }

	//���̓t���[���̃Q�b�^
	const int& GetInputFrame(HANDLE_INPUT_TAG Tag)
	{
		return m_inputFrame[static_cast<int>(Tag)];
	}
	//��x�{�^���𗣂��Ă��邩�̃t���O�Q�b�^
	const bool& GetOffFlg(HANDLE_INPUT_TAG Tag)
	{
		return m_off[static_cast<int>(Tag)];
	}
};

