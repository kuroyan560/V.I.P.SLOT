#include "PlayerStatus.h"
#include"UsersInput.h"

PLAYER_STATUS_TAG PlayerStatus::WaitUpdate(const PlayerParameterForStatus& Parameters)
{
	//移動入力
	if (m_leftStickInputFrame)return PLAYER_STATUS_TAG::MOVE;

	//通常攻撃入力
	if (GetInputFrame(HANDLE_INPUT_TAG::ATTACK))return PLAYER_STATUS_TAG::ATTACK;

	//ジャンプ入力
	if (Parameters.m_onGround && GetInputFrame(HANDLE_INPUT_TAG::JUMP))return PLAYER_STATUS_TAG::JUMP;

	//ガード入力
	if (GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))return PLAYER_STATUS_TAG::GUARD;

	//アビリティ発動入力
	if (m_off[static_cast<int>(HANDLE_INPUT_TAG::ABILITY)] && GetInputFrame(HANDLE_INPUT_TAG::ABILITY))
	{
		m_off[static_cast<int>(HANDLE_INPUT_TAG::ABILITY)] = false;
		return PLAYER_STATUS_TAG::INVOKE_ABILITY;
	}

	//ラッシュ入力
	if (GetInputFrame(HANDLE_INPUT_TAG::RUSH))
	{
		//単発ラッシュのチャージ
		if (Parameters.m_markingNum == 0)return PLAYER_STATUS_TAG::CHARGE;
		//ラッシュ発動
		else return PLAYER_STATUS_TAG::RUSH;
	}

	//マーキング入力
	if (GetInputFrame(HANDLE_INPUT_TAG::MARKING))return PLAYER_STATUS_TAG::MARKING;

	//変化なし
	return PLAYER_STATUS_TAG::WAIT;
}

PLAYER_STATUS_TAG PlayerStatus::MoveUpdate(const PlayerParameterForStatus& Parameters)
{
	//通常攻撃入力
	if (GetInputFrame(HANDLE_INPUT_TAG::ATTACK))return PLAYER_STATUS_TAG::ATTACK;

	//ガード入力
	if (GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN)
		&& GetOffFlg(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))return PLAYER_STATUS_TAG::GUARD;

	//マーキング入力
	if (GetInputFrame(HANDLE_INPUT_TAG::MARKING))return PLAYER_STATUS_TAG::MARKING;

	//ジャンプ入力
	if (Parameters.m_onGround && GetInputFrame(HANDLE_INPUT_TAG::JUMP))return PLAYER_STATUS_TAG::JUMP;

	//移動入力がなくなった
	if (!m_leftStickInputFrame)return PLAYER_STATUS_TAG::WAIT;

	//ラッシュ入力
	if (GetInputFrame(HANDLE_INPUT_TAG::RUSH))
	{
		//単発ラッシュのチャージ
		if (Parameters.m_markingNum == 0)return PLAYER_STATUS_TAG::CHARGE;
		//ラッシュ発動
		else return PLAYER_STATUS_TAG::RUSH;
	}

	//アビリティ発動入力
	if (m_off[static_cast<int>(HANDLE_INPUT_TAG::ABILITY)] && GetInputFrame(HANDLE_INPUT_TAG::ABILITY))
	{
		m_off[static_cast<int>(HANDLE_INPUT_TAG::ABILITY)] = false;
		return PLAYER_STATUS_TAG::INVOKE_ABILITY;
	}

	//変化なし
	return PLAYER_STATUS_TAG::MOVE;
}

PLAYER_STATUS_TAG PlayerStatus::JumpUpdate(const PlayerParameterForStatus& Parameters)
{
	//通常攻撃入力
	if (GetInputFrame(HANDLE_INPUT_TAG::ATTACK))return PLAYER_STATUS_TAG::ATTACK;

	//ガード入力
	if (GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN)
		&& GetOffFlg(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))return PLAYER_STATUS_TAG::GUARD;

	//地面に足がついた
	if (Parameters.m_onGround)return PLAYER_STATUS_TAG::WAIT;

	//変化なし
	return PLAYER_STATUS_TAG::JUMP;
}

void PlayerStatus::Update(const UsersInput& Input, const ControllerConfig& Controller, const PlayerParameterForStatus& Parameters)
{
	//ボタンの入力フレーム記録
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

	//１フレーム前の状態を記録
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
		//攻撃終了フラグ
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
		//ガード入力がなくなった
		if (!GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))m_status = PLAYER_STATUS_TAG::WAIT;
		//移動入力 && 地面に足がついてる
		if (m_leftStickInputFrame && Parameters.m_onGround && !m_jumpGuard)m_status = PLAYER_STATUS_TAG::DODGE;	//回避
		break;
	}

	case PLAYER_STATUS_TAG::DODGE:
	{
		//回避終了フラグ
		if (Parameters.m_dodgeFinish)
		{
			//移動入力
			if (m_leftStickInputFrame)m_status = PLAYER_STATUS_TAG::RUN;	//ダッシュに移行
			else m_status = PLAYER_STATUS_TAG::WAIT;
		}
		break;
	}

	case PLAYER_STATUS_TAG::RUN:
	{
		//攻撃入力
		if (GetInputFrame(HANDLE_INPUT_TAG::ATTACK))m_status = PLAYER_STATUS_TAG::ATTACK;
		//ジャンプ入力
		else if (GetInputFrame(HANDLE_INPUT_TAG::JUMP))m_status = PLAYER_STATUS_TAG::JUMP;
		//移動入力なし
		else if (!m_leftStickInputFrame)m_status = PLAYER_STATUS_TAG::WAIT;
		//走り入力なし
		else if (!GetInputFrame(HANDLE_INPUT_TAG::GUARD_DODGE_RUN))m_status = PLAYER_STATUS_TAG::MOVE;
		break;
	}
	case PLAYER_STATUS_TAG::MARKING:
	{
		//マーキング最大数到達 || マーキング入力なし
		if (Parameters.m_maxMarking || !GetInputFrame(HANDLE_INPUT_TAG::MARKING))m_status = PLAYER_STATUS_TAG::WAIT;
		break;
	}
	case PLAYER_STATUS_TAG::CHARGE:
	{
		//単発ラッシュ発動
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

	//一度も入力を離してないので状態遷移取り消し
	if ((m_status == PLAYER_STATUS_TAG::ATTACK && !m_off[static_cast<int>(HANDLE_INPUT_TAG::ATTACK)])
		|| (m_status == PLAYER_STATUS_TAG::JUMP && !m_off[static_cast<int>(HANDLE_INPUT_TAG::JUMP)])
		|| (m_status == PLAYER_STATUS_TAG::GUARD && !m_off[static_cast<int>(HANDLE_INPUT_TAG::GUARD_DODGE_RUN)]))
	{
		m_status = m_oldStatus;
	}

	//ステータス切り替え時、フラグ更新
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
