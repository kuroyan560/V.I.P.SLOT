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
#include"Block.h"

Player::Player(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Player;

	//���f���ǂݍ���
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	//��_���[�W�q�b�g�X�g�b�vSE
	int onDamagedHitStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_hitStop.wav",0.5f);

	//��_���[�WSE
	int onDamagedSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_onTrigger.wav",0.4f);

	//�G�̓��݂�SE
	int onStepEnemySE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_step.wav",0.8f);

	//�u���b�N�j��SE
	int blockBrokenSE = AudioApp::Instance()->LoadAudio("resource/user/sound/block_broken.wav", 0.5f);

	/*--- �R���C�_�[�p�v���~�e�B�u���� ---*/

	//���f���S�̂𕢂���
	std::shared_ptr<CollisionPrimitive>bodySphereCol = std::make_shared<CollisionSphere>(
		1.2f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -0.2f, 0.0f));

	//�����̓����蔻�苅
	std::shared_ptr<CollisionPrimitive>footSphereCol = std::make_shared<CollisionSphere>(
		1.0f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -1.5f, 0.0f));

	/*--- �R�[���o�b�N���� ---*/
	//��_���[�W�R�[���o�b�N
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, onDamagedHitStopSE, onDamagedSE);
	//�W�����v���񕜃R�[���o�b�N
	m_callBackWithBlock = std::make_shared<CallBackWithBlock>(this, arg_collisionMgr, blockBrokenSE);

	/*--- �R���C�_�[�����i���菇�j ---*/

	std::vector<std::shared_ptr<Collider>>colliders;
	
	//���f���S�̂𕢂��R���C�_�[
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			bodySphereCol
		};
		m_bodyCollider = std::make_shared<Collider>();
		m_bodyCollider->Generate("Player_Body", "Player", coverModelPrimitiveArray);
		m_bodyCollider->SetParentObject(this);
		m_bodyCollider->SetParentTransform(&m_modelObj->m_transform);

		//��_���[�W�R�[���o�b�N�A�^�b�`
		m_bodyCollider->SetCallBack("Enemy", m_damegedCallBack.get());
		//�u���b�N�ɐG�ꂽ�ۂ̃R�[���o�b�N
		m_bodyCollider->SetCallBack("Block", m_callBackWithBlock.get());
		colliders.emplace_back(m_bodyCollider);
	}

	/*--- �R���C�_�[�z��o�^ ---*/
	arg_collisionMgr.lock()->Register(colliders);
}

void Player::Init(std::weak_ptr<GameCamera>arg_cam)
{
	using namespace ConstParameter::Player;

	//HP������
	m_hp = MAX_HP;

	//�X�^�[�g�ʒu�Ɉړ�
	m_modelObj->m_transform.SetPos(INIT_POS);

	//���͂ɂ��ړ�����������
	m_inputMoveVec = { 0,0,0 };

	//�ړ����x
	m_move = { 0,0,0 };

	//�����x
	m_accel = { 0,0,0 };

	//���������Z�b�g
	m_coinVault.Set(300000);

	//��_���[�W�R�[���o�b�N
	m_damegedCallBack->Init(arg_cam);

	//�U���q�b�g�G�t�F�N�g
	m_attackHitEffect.Init();
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	using namespace ConstParameter::Environment;

//���͏��擾
	const auto& input = *UsersInput::Instance();

	//���͐ݒ�
	switch (m_inputConfig)
	{
	//�L�[�{�[�h����
	case Player::INPUT_CONFIG::KEY_BOARD:
	{
		//��������
		if (input.KeyInput(DIK_LEFT))m_inputMoveVec.x = -1.0f;
		else if (input.KeyInput(DIK_RIGHT))m_inputMoveVec.x = 1.0f;
		if (input.KeyInput(DIK_UP))m_inputMoveVec.y = 1.0f;
		else if (input.KeyInput(DIK_DOWN))m_inputMoveVec.y = -1.0f;
		break;
	}

	//�R���g���[���[����
	case Player::INPUT_CONFIG::CONTROLLER:
	{
		//��������
		auto inputVec = input.GetLeftStickVec(0);
		m_inputMoveVec.x = inputVec.x;
		m_inputMoveVec.y = -inputVec.y;	//Y���t
		break;
	}

	//���̑��̓��͐ݒ�ɂȂ��Ă����ꍇ�G���[
	default:
		assert(0);
		break;
	}

//���͏������ɑ���
	//����
	m_move += m_accel;

	//�ړ��ʉ��Z
	auto pos = m_modelObj->m_transform.GetPos();
	pos += m_move * arg_timeScale.GetTimeScale();

	//�����x����
	m_accel = KuroMath::Lerp(m_accel, m_inputMoveVec * INPUT_ACCEL_POWER, ACCEL_LERP_RATE);

	//�ړ��ʌ���
	m_move = KuroMath::Lerp(m_move, Vec3<float>(0, 0, 0), MOVE_DAMP_RATE);

	//�����߂��i���j
	if (pos.y < 0.0f)
	{
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

	//��_���[�W�R�[���o�b�N
	m_damegedCallBack->Update(arg_timeScale);

	//�U���q�b�g�G�t�F�N�g
	m_attackHitEffect.Update(arg_timeScale.GetTimeScale());
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (!m_damegedCallBack->GetIsDraw())return;

	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
}

void Player::EffectDraw(std::weak_ptr<Camera> arg_cam)
{
	m_attackHitEffect.Draw(arg_cam);
}

Vec3<float> Player::GetCenterPos() const
{
	return m_modelObj->m_transform.GetPos() + ConstParameter::Player::FIX_MODEL_CENTER_OFFSET;
}

void Player::DamagedCallBack::OnCollisionTrigger(const Vec3<float>& arg_inter, 
	std::weak_ptr<Collider>arg_otherCollider)
{
	using namespace ConstParameter::Player;

	//���G���Ԓ���
	if (!m_invincibleTimer.IsTimeUp())return;

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
}

void Player::CallBackWithBlock::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	m_parent->m_attackHitEffect.Emit(arg_inter);
	AudioApp::Instance()->PlayWaveDelay(m_brokenSE, 3);

	auto block = arg_otherCollider.lock()->GetParentObject<Block>();
	auto accelVec = block->m_transform.GetPos() - m_parent->GetCenterPos();

	const float ACCEL_POWER = 0.7f;
	m_parent->m_accel = accelVec.GetNormal() * ACCEL_POWER;
}