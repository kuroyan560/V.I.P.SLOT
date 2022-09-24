#include "Player.h"
#include"Object.h"
#include"UsersInput.h"
#include"ConstParameters.h"

Player::Player()
{
	//���f���ǂݍ���
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");
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
}

void Player::Update()
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
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam)
{
	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *Cam.lock(), 1.0f, AlphaBlendMode_None);

}
