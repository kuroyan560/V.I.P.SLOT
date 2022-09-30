#include "Player.h"
#include"Object.h"
#include"UsersInput.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"SlotMachine.h"
#include"TimeScale.h"
#include"Collision.h"
#include"Collider.h"
#include"CollisionManager.h"

Player::Player(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//���f���ǂݍ���
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	//BET��SE�ǂݍ���
	m_betSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav",0.15f);

	/*--- �R���C�_�[�p�v���~�e�B�u���� ---*/
	//���f�������ɍ��킹�邽�߂̃I�t�Z�b�g�l
	const Vec3<float>FIX_MODEL_CENTER_OFFSET = { 0.0f,2.0f,ConstParameter::Environment::FIELD_DEPTH_MODEL_OFFSET };

	//���f���S�̂𕢂���
	std::shared_ptr<CollisionPrimitive>bodySphereCol = std::make_shared<CollisionSphere>(
		1.4f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -0.3f, 0.0f),
		&m_modelObj->m_transform);

	/*--- �R�[���o�b�N���� ---*/
	//��_���[�W�R�[���o�b�N
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this);

	/*--- ���f���S�̂𕢂��R���C�_�[���� ---*/
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			bodySphereCol
		};
		std::shared_ptr<Collider>bodyCollider = std::make_shared<Collider>("Player_Damaged", coverModelPrimitiveArray);

		//��_���[�W�R�[���o�b�N�A�^�b�`
		bodyCollider->SetCallBack(m_damegedCallBack, arg_collisionMgr.lock()->GetAttribute("Enemy"));
		m_colliders.emplace_back(bodyCollider);
	}

	/*--- �R���C�_�[�z��o�^ ---*/
	arg_collisionMgr.lock()->Register("Player", m_colliders);
}

void Player::Init()
{
	using namespace ConstParameter::Player;

	//HP������
	m_hp = MAX_HP;

	//�X�^�[�g�ʒu�Ɉړ�
	m_modelObj->m_transform.SetPos(INIT_POS);

	//�ړ����x
	m_move = { 0,0,0 };

	//�������x������
	m_fallSpeed = 0.0f;

	//�ڒn�t���O������
	m_isOnGround = true;

	//���������Z�b�g
	m_coinVault.Set(300000);

	//BET�̃X�p���v���p�^�C�}�[
	m_betTimer.Reset(0);

	//�A��BET�̌v���p�^�C�}�[
	m_consecutiveBetTimer.Reset(UNTIL_MAX_SPEED_BET_TIME);

	//��_���[�W�R�[���o�b�N
	m_damegedCallBack->Init();
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, const TimeScale& arg_timeScale)
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
		m_fallSpeed -= ConstParameter::Player::STRONG_GRAVITY * arg_timeScale.GetTimeScale();
	}
	else
	{
		m_fallSpeed -= ConstParameter::Player::WEAK_GRAVITY * arg_timeScale.GetTimeScale();
	}

	//�������x����
	if (m_fallSpeed < ConstParameter::Player::FALL_SPEED_MIN)m_fallSpeed = ConstParameter::Player::FALL_SPEED_MIN;

	//�ړ��ʉ��Z
	auto pos = m_modelObj->m_transform.GetPos();
	pos += m_move * arg_timeScale.GetTimeScale();

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
		if (m_betTimer.UpdateTimer(arg_timeScale.GetTimeScale()))
		{
			//�X���b�g�}�V����BET
			arg_slotMachine.lock()->Bet(ConstParameter::Player::PASS_COIN_NUM, m_modelObj->m_transform);

			//BET�X�p���v�Z
			int betSpan = KuroMath::Lerp(ConstParameter::Player::BET_SPEED_MIN_SPAN, ConstParameter::Player::BET_SPEED_MAX_SPAN,
				m_consecutiveBetTimer.GetTimeRate());

			//����BET����܂ł̎���
			m_betTimer.Reset(betSpan);

			//BET��SE�Đ�
			AudioApp::Instance()->PlayWave(m_betSE);
		}

		//�A��BET�̎��Ԍv��
		m_consecutiveBetTimer.UpdateTimer(arg_timeScale.GetTimeScale());
	}
	//���̓��̓g���K�[���͑��R�C������
	else
	{
		m_betTimer.Reset(0);
		m_consecutiveBetTimer.Reset();
	}

	//��_���[�W�R�[���o�b�N
	m_damegedCallBack->Update(arg_timeScale.GetTimeScale());
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
}

void Player::DamagedCallBack::OnCollision(const Vec3<float>& arg_inter, const unsigned char& arg_otherAttribute, const CollisionManager& arg_collisionMgr)
{
	//���G���Ԓ���
	if (!m_invincibleTimer.IsTimeUp())return;

	m_parent->m_hp--;
	printf("Player : Damaged : remain hp %d\n", m_parent->m_hp);

	m_invincibleTimer.Reset(ConstParameter::Player::INVINCIBLE_TIME_WHEN_DAMAGED);
}