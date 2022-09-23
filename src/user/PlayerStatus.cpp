#include "PlayerStatus.h"
#include"UsersInput.h"

PLAYER_STATUS_TAG PlayerStatus::WaitUpdate(const PlayerParameterForStatus& Parameters)
{
	//�ړ�����
	if (m_leftStickInputFrame)return PLAYER_STATUS_TAG::MOVE;

	//�ʏ�U������
	if (GetInputFrame(HANDLE_INPUT_TAG::ATTACK))return PLAYER_STATUS_TAG::ATTACK;

	//�W�����v����
	if (Parameters.m_onGround && GetInputFrame(HANDLE_INPUT_TAG::JUMP))return PLAYER_STATUS_TAG::JUMP;

	//�K�[�h����
	if (GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))return PLAYER_STATUS_TAG::GUARD;

	//�A�r���e�B��������
	if (m_off[static_cast<int>(HANDLE_INPUT_TAG::ABILITY)] && GetInputFrame(HANDLE_INPUT_TAG::ABILITY))
	{
		m_off[static_cast<int>(HANDLE_INPUT_TAG::ABILITY)] = false;
		return PLAYER_STATUS_TAG::INVOKE_ABILITY;
	}

	//���b�V������
	if (GetInputFrame(HANDLE_INPUT_TAG::RUSH))
	{
		//�P�����b�V���̃`���[�W
		if (Parameters.m_markingNum == 0)return PLAYER_STATUS_TAG::CHARGE;
		//���b�V������
		else return PLAYER_STATUS_TAG::RUSH;
	}

	//�}�[�L���O����
	if (GetInputFrame(HANDLE_INPUT_TAG::MARKING))return PLAYER_STATUS_TAG::MARKING;

	//�ω��Ȃ�
	return PLAYER_STATUS_TAG::WAIT;
}

PLAYER_STATUS_TAG PlayerStatus::MoveUpdate(const PlayerParameterForStatus& Parameters)
{
	//�ʏ�U������
	if (GetInputFrame(HANDLE_INPUT_TAG::ATTACK))return PLAYER_STATUS_TAG::ATTACK;

	//�K�[�h����
	if (GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN)
		&& GetOffFlg(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))return PLAYER_STATUS_TAG::GUARD;

	//�}�[�L���O����
	if (GetInputFrame(HANDLE_INPUT_TAG::MARKING))return PLAYER_STATUS_TAG::MARKING;

	//�W�����v����
	if (Parameters.m_onGround && GetInputFrame(HANDLE_INPUT_TAG::JUMP))return PLAYER_STATUS_TAG::JUMP;

	//�ړ����͂��Ȃ��Ȃ���
	if (!m_leftStickInputFrame)return PLAYER_STATUS_TAG::WAIT;

	//���b�V������
	if (GetInputFrame(HANDLE_INPUT_TAG::RUSH))
	{
		//�P�����b�V���̃`���[�W
		if (Parameters.m_markingNum == 0)return PLAYER_STATUS_TAG::CHARGE;
		//���b�V������
		else return PLAYER_STATUS_TAG::RUSH;
	}

	//�A�r���e�B��������
	if (m_off[static_cast<int>(HANDLE_INPUT_TAG::ABILITY)] && GetInputFrame(HANDLE_INPUT_TAG::ABILITY))
	{
		m_off[static_cast<int>(HANDLE_INPUT_TAG::ABILITY)] = false;
		return PLAYER_STATUS_TAG::INVOKE_ABILITY;
	}

	//�ω��Ȃ�
	return PLAYER_STATUS_TAG::MOVE;
}

PLAYER_STATUS_TAG PlayerStatus::JumpUpdate(const PlayerParameterForStatus& Parameters)
{
	//�ʏ�U������
	if (GetInputFrame(HANDLE_INPUT_TAG::ATTACK))return PLAYER_STATUS_TAG::ATTACK;

	//�K�[�h����
	if (GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN)
		&& GetOffFlg(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))return PLAYER_STATUS_TAG::GUARD;

	//�n�ʂɑ�������
	if (Parameters.m_onGround)return PLAYER_STATUS_TAG::WAIT;

	//�ω��Ȃ�
	return PLAYER_STATUS_TAG::JUMP;
}

void PlayerStatus::Update(const UsersInput& Input, const ControllerConfig& Controller, const PlayerParameterForStatus& Parameters)
{
	//�{�^���̓��̓t���[���L�^
	for (int tagIdx = 0; tagIdx < static_cast<int>(HANDLE_INPUT_TAG::NUM); ++tagIdx)
	{
		if (Controller.GetHandleInput(Input, tagIdx))
		{
			m_inputFrame[tagIdx]++;
		}
		else
		{
			m_inputFrame[tagIdx] = 0;
			m_off[tagIdx] = true;
		}
	}
	if (!Controller.GetMoveVec(Input).IsZero())m_leftStickInputFrame++;
	else m_leftStickInputFrame = 0;

	//�P�t���[���O�̏�Ԃ��L�^
	m_oldStatus = m_status;

	switch (m_status)
	{
	case PLAYER_STATUS_TAG::WAIT:
		m_status = WaitUpdate(Parameters);
		break;

	case PLAYER_STATUS_TAG::MOVE:
		m_status = MoveUpdate(Parameters);
		break;

	case PLAYER_STATUS_TAG::ATTACK:
		//�U���I���t���O
		if (Parameters.m_attackFinish)
		{
			m_status = PLAYER_STATUS_TAG::WAIT;
			m_off[static_cast<int>(HANDLE_INPUT_TAG::ATTACK)] = false;
		}
		break;

	case PLAYER_STATUS_TAG::JUMP:
		m_status = JumpUpdate(Parameters);
		break;

	case PLAYER_STATUS_TAG::GUARD:
	{
		//�K�[�h���͂��Ȃ��Ȃ���
		if (!GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))m_status = PLAYER_STATUS_TAG::WAIT;
		//�ړ����� && �n�ʂɑ������Ă�
		if (m_leftStickInputFrame && Parameters.m_onGround && !m_jumpGuard)m_status = PLAYER_STATUS_TAG::DODGE;	//���
		break;
	}

	case PLAYER_STATUS_TAG::DODGE:
	{
		//����I���t���O
		if (Parameters.m_dodgeFinish)
		{
			//�ړ�����
			if (m_leftStickInputFrame)m_status = PLAYER_STATUS_TAG::RUN;	//�_�b�V���Ɉڍs
			else m_status = PLAYER_STATUS_TAG::WAIT;
		}
		break;
	}

	case PLAYER_STATUS_TAG::RUN:
	{
		//�U������
		if (GetInputFrame(HANDLE_INPUT_TAG::ATTACK))m_status = PLAYER_STATUS_TAG::ATTACK;
		//�W�����v����
		else if (GetInputFrame(HANDLE_INPUT_TAG::JUMP))m_status = PLAYER_STATUS_TAG::JUMP;
		//�ړ����͂Ȃ�
		else if (!m_leftStickInputFrame)m_status = PLAYER_STATUS_TAG::WAIT;
		//������͂Ȃ�
		else if (!GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))m_status = PLAYER_STATUS_TAG::MOVE;
		break;
	}
	case PLAYER_STATUS_TAG::MARKING:
	{
		//�}�[�L���O�ő吔���B || �}�[�L���O���͂Ȃ�
		if (Parameters.m_maxMarking || !GetInputFrame(HANDLE_INPUT_TAG::MARKING))m_status = PLAYER_STATUS_TAG::WAIT;
		break;
	}
	case PLAYER_STATUS_TAG::CHARGE:
	{
		//�P�����b�V������
		if (GetInputFrame(HANDLE_INPUT_TAG::MARKING))m_status = PLAYER_STATUS_TAG::RUSH;
		if (!GetInputFrame(HANDLE_INPUT_TAG::RUSH))m_status = PLAYER_STATUS_TAG::WAIT;
		break;
	}
	case PLAYER_STATUS_TAG::RUSH:
	{
		if (Parameters.m_rushFinish)m_status = PLAYER_STATUS_TAG::WAIT;
		break;
	}
	case PLAYER_STATUS_TAG::INVOKE_ABILITY:
	{
		if(Parameters.m_abilityFinish)m_status = PLAYER_STATUS_TAG::WAIT;
		break;
	}
	case PLAYER_STATUS_TAG::OUT_OF_CONTROL:
		break;
	}

	//��x�����͂𗣂��ĂȂ��̂ŏ�ԑJ�ڎ�����
	if ((m_status == PLAYER_STATUS_TAG::ATTACK && !m_off[static_cast<int>(HANDLE_INPUT_TAG::ATTACK)])
		|| (m_status == PLAYER_STATUS_TAG::JUMP && !m_off[static_cast<int>(HANDLE_INPUT_TAG::JUMP)])
		|| (m_status == PLAYER_STATUS_TAG::GUARD && !m_off[static_cast<int>(HANDLE_INPUT_TAG::GUARD_DODGE_RUN)]))
	{
		m_status = m_oldStatus;
	}

	//�X�e�[�^�X�؂�ւ����A�t���O�X�V
	if (m_status != m_oldStatus)
	{
		if (m_status == PLAYER_STATUS_TAG::GUARD)
		{
			m_jumpGuard = !Parameters.m_onGround;
			m_off[static_cast<int>(HANDLE_INPUT_TAG::GUARD_DODGE_RUN)] = false;
		}
		else if (m_status == PLAYER_STATUS_TAG::JUMP)
		{
			m_off[static_cast<int>(HANDLE_INPUT_TAG::JUMP)] = false;
		}
	}
}
