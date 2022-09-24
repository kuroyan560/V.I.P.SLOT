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

	//���W�p�x������
	m_posAngle = Angle(0);

	//���W�p�ړ��ʏ�����
	m_moveAngle = Angle(0);

	//�������x������
	m_fallAccel = 0.0f;

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
	const Angle ANGLE_SPEED = Angle::ConvertToRadian(0.85f);
	const float MOVE_LERP_RATE = 0.3f;
	if (0.0f < stickVec.x)
	{
		m_moveAngle = KuroMath::Lerp(m_moveAngle, -ANGLE_SPEED, MOVE_LERP_RATE);
	}
	else if (stickVec.x < 0.0f)
	{
		m_moveAngle = KuroMath::Lerp(m_moveAngle, ANGLE_SPEED, MOVE_LERP_RATE);
	}
	else
	{
		m_moveAngle = KuroMath::Lerp(m_moveAngle, 0.0f, MOVE_LERP_RATE);
	}

	//�W�����v
	const float JUMP_POWER = 0.62f;
	if (jumpTrigger && m_isOnGround)
	{
		m_fallAccel = JUMP_POWER;
		m_isOnGround = false;
	}

	//��������
	m_fallSpeed += m_fallAccel;

	//�������x�v�Z�i�W�����v���Ɨ������ŏd�͕ω��A�f�����W�����v �� �������~���j
	const float STRONG_GRAVITY = 0.34f;
	const float WEAK_GRAVITY = 0.00005f;
	if (0.0f < m_fallAccel)
	{
		m_fallAccel -= STRONG_GRAVITY;
	}
	else
	{
		m_fallAccel -= WEAK_GRAVITY;
	}
	//���������x����
	const float FALL_SPEED_MIN = -0.2f;
	if (m_fallAccel < FALL_SPEED_MIN)m_fallAccel = FALL_SPEED_MIN;

	//���݂̍��W�擾
	auto pos = m_modelObj->m_transform.GetPos();
	//�����𔽉f
	pos.y += m_fallSpeed;
	//���ړ��̔��f
	m_posAngle += m_moveAngle;
	auto xzPos = KuroMath::TransformVec3(m_startPos, { 0,1,0 }, m_posAngle);
	pos.x = xzPos.x;
	pos.z = xzPos.z;

	//�����߂�
	if (pos.y < 0.0f)
	{
		pos.y = 0.0f;
		m_fallAccel = 0.0f;
		m_fallSpeed = 0.0f;
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
