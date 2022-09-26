#include "Player.h"
#include"Object.h"
#include"UsersInput.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"SlotMachine.h"

Player::Player()
{
	//���f���ǂݍ���
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	//BET��SE�ǂݍ���
	m_betSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav",0.15f);
}

void Player::Init()
{
	//�X�^�[�g�ʒu�Ɉړ�
	m_modelObj->m_transform.SetPos(ConstParameter::Player::INIT_POS);

	//�ړ����x
	m_move = { 0,0,0 };

	//�������x������
	m_fallSpeed = 0.0f;

	//�ڒn�t���O������
	m_isOnGround = true;

	//���������Z�b�g
	m_coinVault.Init(300000);

	//BET�̃X�p���v���p�^�C�}�[
	m_betTimer = 0;

	//�A��BET�̌v���p�^�C�}�[
	m_consecutiveBetTimer = 0;
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine)
{
//���͏��擾
	const auto& input = *UsersInput::Instance();

	//���X�e�B�b�N�i�ړ��j����
	const auto stickVec = input.GetLeftStickVec(0);

	//A�{�^���i�W�����v�j����
	const bool jumpTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::A);

	//X�{�^���i�O���ɃV���b�g�j����
	const bool shotTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::X);

	//LB�{�^���iBET�j����
	const bool betInput = input.ControllerInput(0, XBOX_BUTTON::LB);

//���͏������ɑ���
	//���ړ�
	if (0.0f < stickVec.x)
	{
		m_move.x = KuroMath::Lerp(m_move.x, ConstParameter::Player::MOVE_SPEED, ConstParameter::Player::MOVE_LERP_RATE);
	}
	else if (stickVec.x < 0.0f)
	{
		m_move.x = KuroMath::Lerp(m_move.x, -ConstParameter::Player::MOVE_SPEED, ConstParameter::Player::MOVE_LERP_RATE);
	}
	else
	{
		m_move.x = KuroMath::Lerp(m_move.x, 0.0f, ConstParameter::Player::MOVE_LERP_RATE);
	}

	//�W�����v
	if (jumpTrigger && m_isOnGround)
	{
		m_fallSpeed += ConstParameter::Player::JUMP_POWER;
		m_isOnGround = false;
	}

	//�����i�W�����v���Ɨ������ŏd�͕ω��A�f�����W�����v �� �������~���j
	m_move.y += m_fallSpeed;
	if (0.0f < m_fallSpeed)
	{
		m_fallSpeed -= ConstParameter::Player::STRONG_GRAVITY;
	}
	else
	{
		m_fallSpeed -= ConstParameter::Player::WEAK_GRAVITY;
	}

	//�������x����
	if (m_fallSpeed < ConstParameter::Player::FALL_SPEED_MIN)m_fallSpeed = ConstParameter::Player::FALL_SPEED_MIN;

	//�ړ��ʉ��Z
	auto pos = m_modelObj->m_transform.GetPos();
	pos += m_move;

	//�����߂��i���j
	if (pos.y < 0.0f)
	{
		pos.y = 0.0f;
		m_fallSpeed = 0.0f;
		m_move.y = 0.0f;
		m_isOnGround = true;
	}

	//�����߂��i�X�e�[�W�[�j
	if (pos.x < -ConstParameter::Environment::FIELD_WIDTH_HALF)
	{
		pos.x = -ConstParameter::Environment::FIELD_WIDTH_HALF;
		m_move.x = 0.0f;
	}
	else if(ConstParameter::Environment::FIELD_WIDTH_HALF < pos.x)
	{
		pos.x = ConstParameter::Environment::FIELD_WIDTH_HALF;
		m_move.x = 0.0f;
	}

	//�X�V�������W�̔��f
	m_modelObj->m_transform.SetPos(pos);

	//�R�C����BET
	if (betInput)
	{
		//�R�C������
		if (m_betTimer <= 0)
		{
			//�X���b�g�}�V����BET
			arg_slotMachine.lock()->Bet(m_coinVault, ConstParameter::Player::PASS_COIN_NUM);

			//BET�X�p���v�Z
			int betSpan = KuroMath::Lerp(ConstParameter::Player::BET_SPEED_MIN_SPAN, ConstParameter::Player::BET_SPEED_MAX_SPAN,
				m_consecutiveBetTimer, ConstParameter::Player::UNTIL_MAX_SPEED_BET_TIME);

			//����BET����܂ł̎���
			m_betTimer = betSpan;

			//BET��SE�Đ�
			AudioApp::Instance()->PlayWave(m_betSE);
		}
		//�R�C�������C���^�[�o��
		else
		{
			m_betTimer--;
		}

		//�A��BET�̎��Ԍv��
		if (m_consecutiveBetTimer < ConstParameter::Player::UNTIL_MAX_SPEED_BET_TIME)
		{
			m_consecutiveBetTimer++;
		}
	}
	//���̓��̓g���K�[���͑��R�C������
	else
	{
		m_betTimer = 0;
		m_consecutiveBetTimer = 0;
	}
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);

}
