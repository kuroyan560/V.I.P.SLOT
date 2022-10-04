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
#include"GameCamera.h"

void Player::Jump()
{
	m_move.y = 0.0f;
	m_fallSpeed = ConstParameter::Player::JUMP_POWER;
}

Player::Player(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Player;

	//���f���ǂݍ���
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	//�W�����vSE�ǂݍ���
	m_jumpSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_jump.wav",0.6f);

	//BET��SE�ǂݍ���
	m_betSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav",0.15f);

	//��_���[�W�q�b�g�X�g�b�vSE
	int onDamagedHitStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_hitStop.wav",0.5f);

	//��_���[�WSE
	int onDamagedSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_onTrigger.wav",0.4f);

	//�G�̓��݂�SE
	int onStepEnemySE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_step.wav",0.8f);

	/*--- �R���C�_�[�p�v���~�e�B�u���� ---*/

	//���f���S�̂𕢂���
	std::shared_ptr<CollisionPrimitive>bodySphereCol = std::make_shared<CollisionSphere>(
		1.2f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -0.2f, 0.0f),
		&m_modelObj->m_transform);

	//�����̓����蔻�苅
	std::shared_ptr<CollisionPrimitive>footSphereCol = std::make_shared<CollisionSphere>(
		1.0f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -1.5f, 0.0f),
		&m_modelObj->m_transform);

	/*--- �R�[���o�b�N���� ---*/
	//��_���[�W�R�[���o�b�N
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, onDamagedHitStopSE, onDamagedSE);
	//�U���R�[���o�b�N
	m_attackCallBack = std::make_shared<AttackCallBack>(this, onStepEnemySE);

	/*--- �R���C�_�[�����i���菇�j ---*/

	std::vector<std::shared_ptr<Collider>>colliders;
	
	//�����̃R���C�_�[
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>footPrimitiveArray = { footSphereCol };
		m_footCollider = std::make_shared<Collider>("Player_Foot", footPrimitiveArray);

		//�U���R�[���o�b�N�A�^�b�`
		m_footCollider->SetCallBack(m_attackCallBack, arg_collisionMgr.lock()->GetAttribute("Enemy"));
		colliders.emplace_back(m_footCollider);
	}
	//���f���S�̂𕢂��R���C�_�[
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			bodySphereCol
		};
		m_bodyCollider = std::make_shared<Collider>("Player_Body", coverModelPrimitiveArray);

		//��_���[�W�R�[���o�b�N�A�^�b�`
		m_bodyCollider->SetCallBack(m_damegedCallBack, arg_collisionMgr.lock()->GetAttribute("Enemy"));
		colliders.emplace_back(m_bodyCollider);
	}


	/*--- �R���C�_�[�z��o�^ ---*/
	arg_collisionMgr.lock()->Register("Player", colliders);
}

void Player::Init(std::weak_ptr<GameCamera>arg_cam)
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
	m_damegedCallBack->Init(arg_cam);

	//�U���R�[���o�b�N
	m_attackCallBack->Init();
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	using namespace ConstParameter::Environment;

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
	if (jumpTrigger && m_isOnGround)
	{
		Jump();
		AudioApp::Instance()->PlayWaveDelay(m_jumpSE,3);
		m_isOnGround = false;
	}

	//�����i�W�����v���Ɨ������ŏd�͕ω��A�f�����W�����v �� �������~���j
	m_move.y += m_fallSpeed * arg_timeScale.GetTimeScale();
	if (0.0f < m_fallSpeed)
	{
		m_fallSpeed -= STRONG_GRAVITY * arg_timeScale.GetTimeScale();
	}
	else
	{
		m_fallSpeed -= WEAK_GRAVITY * arg_timeScale.GetTimeScale();
	}

	//�������x����
	if (m_fallSpeed < FALL_SPEED_MIN)m_fallSpeed = FALL_SPEED_MIN;

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
	if (pos.x < -FIELD_WIDTH_HALF)
	{
		pos.x = -FIELD_WIDTH_HALF;
		m_move.x = 0.0f;
	}
	else if(FIELD_WIDTH_HALF < pos.x)
	{
		pos.x = FIELD_WIDTH_HALF;
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
			arg_slotMachine.lock()->Bet(PASS_COIN_NUM, m_modelObj->m_transform);

			//BET�X�p���v�Z
			int betSpan = KuroMath::Lerp(BET_SPEED_MIN_SPAN, BET_SPEED_MAX_SPAN,
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
	m_damegedCallBack->Update(arg_timeScale);

	//�U���R�[���o�b�N
	m_attackCallBack->Update(arg_timeScale.GetTimeScale());
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (!m_damegedCallBack->GetIsDraw())return;

	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
}

void Player::GetCoin(int arg_coinNum)
{
	m_coinVault.Add(arg_coinNum);
}

const Transform& Player::GetTransform()const
{
	return m_modelObj->m_transform;
}

void Player::DamagedCallBack::OnCollision(const Vec3<float>& arg_inter, 
	std::weak_ptr<Collider>arg_otherCollider,
	const unsigned char& arg_otherAttribute, 
	const CollisionManager& arg_collisionMgr)
{
	using namespace ConstParameter::Player;

	//���G���Ԓ���
	if (!m_invincibleTimer.IsTimeUp())return;
	if (m_parent->m_attackCallBack->GetIsInvincible())return;

	//HP����
	m_parent->m_hp--;

	//���G���Ԑݒ�
	m_invincibleTimer.Reset(INVINCIBLE_TIME_ON_DAMAGED);

	//�q�b�g�X�g�b�v
	m_hitStopTimer.Reset(HIT_STOP_TIME_ON_DAMAGED);

	//�q�b�g�X�g�b�vSE�Đ�
	AudioApp::Instance()->PlayWave(m_hitStopSE);

	//�_��
	m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);

	//�J�����U��
	m_cam.lock()->Shake(60, 2, 2.0f, 1.0f);

	//���݂��\�ɂȂ�܂ł̎���
	m_canStepTimer.Reset(CAN_STEP_TIME_AFTER_DAMAGED);

	//����
	m_parent->m_fallSpeed = FALL_SPEED_ON_DAMAGED;
	m_parent->m_move.y = 0.0f;

	printf("Player : Damaged : remain hp %d\n", m_parent->m_hp);
}

void Player::DamagedCallBack::Update(TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	const float& timeScale = arg_timeScale.GetTimeScale();

	//�q�b�g�X�g�b�v�n��
	if (m_hitStopTimer.IsTimeStartOnTrigger())
	{
		arg_timeScale.Set(0.0f);
	}
	//�q�b�g�X�g�b�v�I���
	else if (m_hitStopTimer.IsTimeUpOnTrigger())
	{
		arg_timeScale.Set(1.0f);
		AudioApp::Instance()->PlayWave(m_damageSE);
	}

	m_hitStopTimer.UpdateTimer();

	//���G����
	if (m_invincibleTimer.UpdateTimer(timeScale))
	{
		//�_�ŏI��
		m_isDraw = true;
	}
	else
	{
		//�_��
		if (m_flashTimer.UpdateTimer(timeScale))
		{
			m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);
			m_isDraw = !m_isDraw;
		}
	}
	
	//���݂��\�ɂȂ�܂ł̎���
	m_canStepTimer.UpdateTimer(timeScale);
}

#include"Enemy.h"
void Player::AttackCallBack::OnCollision(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider, const unsigned char& arg_otherAttribute, const CollisionManager& arg_collisionMgr)
{
	using namespace ConstParameter::Player;

	//�W�����v����
	if (m_parent->m_isOnGround)return;

	//��_���[�W����łȂ���
	if (!m_parent->m_damegedCallBack->IsCanStep())return;

	//�v���C���[���Փ˓_����i���݂��ł��Ȃ��j
	if (m_parent->m_modelObj->m_transform.GetPos().y + FIX_MODEL_CENTER_OFFSET.y < arg_inter.y)return;

	auto enemy = arg_otherCollider.lock()->GetParentObject<Enemy>();
	enemy->Damage();

	m_parent->Jump();
	AudioApp::Instance()->PlayWave(m_stepEnemySE);

	//�W�����v�����ɂ�閳�G����
	const int INVINCIBLE_TIME_ON_SUCCESS_ATTACK = 10;
	m_invincibleTimer.Reset(INVINCIBLE_TIME_ON_SUCCESS_ATTACK);
}
