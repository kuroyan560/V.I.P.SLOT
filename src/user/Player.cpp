#include "Player.h"
#include"Object.h"
#include"UsersInput.h"

Player::Player()
{
	//���f���ǂݍ���
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");
}

void Player::Init()
{
	//�X�^�[�g�ʒu�Ɉړ�
	m_modelObj->m_transform.SetPos(m_startPos);

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
	const float MOVE_SPEED = 0.3f;
	const float MOVE_LERP_RATE = 0.3f;
	if (0.0f < stickVec.x)
	{
		m_move.x = KuroMath::Lerp(m_move.x, MOVE_SPEED, MOVE_LERP_RATE);
	}
	else if (stickVec.x < 0.0f)
	{
		m_move.x = KuroMath::Lerp(m_move.x, -MOVE_SPEED, MOVE_LERP_RATE);
	}
	else
	{
		m_move.x = KuroMath::Lerp(m_move.x, 0.0f, MOVE_LERP_RATE);
	}

	//�W�����v
	const float JUMP_POWER = 0.62f;
	if (jumpTrigger && m_isOnGround)
	{
		m_fallSpeed += JUMP_POWER;
		m_isOnGround = false;
	}

	//�����i�W�����v���Ɨ������ŏd�͕ω��A�f�����W�����v �� �������~���j
	m_move.y += m_fallSpeed;
	const float STRONG_GRAVITY = 0.34f;
	const float WEAK_GRAVITY = 0.00005f;
	if (0.0f < m_fallSpeed)
	{
		m_fallSpeed -= STRONG_GRAVITY;
	}
	else
	{
		m_fallSpeed -= WEAK_GRAVITY;
	}

	//�������x����
	const float FALL_SPEED_MIN = -0.2f;
	if (m_fallSpeed < FALL_SPEED_MIN)m_fallSpeed = FALL_SPEED_MIN;

	//�ړ��ʉ��Z
	auto pos = m_modelObj->m_transform.GetPos();
	pos += m_move;

	//�����߂�
	if (pos.y < 0.0f)
	{
		pos.y = 0.0f;
		m_fallSpeed = 0.0f;
		m_move.y = 0.0f;
		m_isOnGround = true;
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
