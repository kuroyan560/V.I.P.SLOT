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
#include"YoYo.h"

void Player::Jump(Vec3<float>* arg_rockOnPos)
{
	m_move.y = 0.0f;
	m_fallSpeed = ConstParameter::Player::JUMP_POWER;
}

Player::Player(std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<GameCamera>arg_cam)
{
	using namespace ConstParameter::Player;

	//���f���ǂݍ���
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	//�W�����vSE�ǂݍ���
	m_jumpSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_jump.wav",0.6f);

	//��_���[�W�q�b�g�X�g�b�vSE
	int onDamagedHitStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_hitStop.wav",0.5f);

	//��_���[�WSE
	int onDamagedSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_onTrigger.wav",0.4f);

	//�G�̓��݂�SE
	int onStepEnemySE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_step.wav",0.8f);

	/*--- �R���C�_�[�p�v���~�e�B�u���� ---*/

	//���f���S�̂𕢂���
	m_bodySphereCol = std::make_shared<CollisionSphere>(
		1.2f,
		Vec3<float>(0.0f, -0.2f, 0.0f));

	//�����̓����蔻�苅
	std::shared_ptr<CollisionPrimitive>footSphereCol = std::make_shared<CollisionSphere>(
		1.0f,
		 Vec3<float>(0.0f, -1.5f, 0.0f));

	/*--- �R�[���o�b�N���� ---*/
	//��_���[�W�R�[���o�b�N
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, arg_cam, onDamagedHitStopSE, onDamagedSE);
	//�W�����v���񕜃R�[���o�b�N
	m_callBackWithBlock = std::make_shared<CallBackWithBlock>(this, arg_collisionMgr);

	/*--- �R���C�_�[�����i���菇�j ---*/

	std::vector<std::shared_ptr<Collider>>colliders;
	
	//���f���S�̂𕢂��R���C�_�[
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			m_bodySphereCol
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

	/*--- ���[���[���� ---*/
	m_yoYo = std::make_shared<YoYo>(arg_collisionMgr, &m_modelObj->m_transform);
	m_yoYo->Awake(2.5f, 1.2f);
}

void Player::Init(int arg_initHp, int arg_initCoinNum)
{
	using namespace ConstParameter::Player;

	//HP������
	m_hp = arg_initHp;

	//���������Z�b�g
	m_coinVault.Set(arg_initCoinNum);

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

	//��_���[�W�R�[���o�b�N
	m_damegedCallBack->Init();

	//���[���[
	m_yoYo->Init();
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	using namespace ConstParameter::Environment;

	Vec3<float>pos = m_modelObj->m_transform.GetPos();

//���͏��擾
	const auto& input = *UsersInput::Instance();

	//�W�����v�̃g���K�[����
	bool jumpTrigger = false;

	//���[���[��������
	bool throwYoyoTrigger = false;

	//�ړ���������
	Vec2<float>moveInput = { 0,0 };

	//���͐ݒ�
	switch (m_inputConfig)
	{
	//�L�[�{�[�h����
	case Player::INPUT_CONFIG::KEY_BOARD:
	{
		//��������
		if (input.KeyInput(DIK_LEFT))moveInput.x = -1.0f;
		else if (input.KeyInput(DIK_RIGHT))moveInput.x = 1.0f;
		//�W�����v����
		jumpTrigger = input.KeyOnTrigger(DIK_UP);
		//���[���[��������
		throwYoyoTrigger = input.KeyOnTrigger(DIK_SPACE);
		break;
	}

	//�R���g���[���[����
	case Player::INPUT_CONFIG::CONTROLLER:
	{
		//��������
		moveInput = input.GetLeftStickVec(0);
		//�W�����v����
		jumpTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::A);
		//���[���[��������
		throwYoyoTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::X);
		break;
	}

	//���̑��̓��͐ݒ�ɂȂ��Ă����ꍇ�G���[
	default:
		assert(0);
		break;
	}

//���͏������ɑ���
	float moveSpeed = m_yoYo->IsActive() ? MOVE_SPEED_WHILE_ATTACK : MOVE_SPEED;

	//���ړ�
	if (0.0f < moveInput.x)
	{
		m_move.x = KuroMath::Lerp(m_move.x, moveSpeed, MOVE_LERP_RATE);
	}
	else if (moveInput.x < 0.0f)
	{
		m_move.x = KuroMath::Lerp(m_move.x, -moveSpeed, MOVE_LERP_RATE);
	}
	else
	{
		m_move.x = KuroMath::Lerp(m_move.x, 0.0f, MOVE_LERP_RATE);
	}

	//�W�����v
	if (jumpTrigger && m_isOnGround)
	{
		Jump();
		AudioApp::Instance()->PlayWaveDelay(m_jumpSE, 3);
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
	pos += m_move * arg_timeScale.GetTimeScale();

	//�����߂��i���j
	if (pos.y < FIELD_FLOOR_TOP_SURFACE_HEIGHT + MODEL_SIZE.y / 2.0f)
	{
		pos.y = FIELD_FLOOR_TOP_SURFACE_HEIGHT + MODEL_SIZE.y / 2.0f;
		m_fallSpeed = 0.0f;
		m_move.y = 0.0f;
		m_isOnGround = true;
	}

	//�����߂��i�X�e�[�W�[�j
	const float FIELD_WIDTH_HALF = FIELD_FLOOR_SIZE.x / 2.0f;
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

//���[���[�i�U���j
	//������
	if (throwYoyoTrigger)
	{
		m_yoYo->Throw(moveInput);
	}

	//���[���[
	m_yoYo->Update(arg_timeScale);
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (m_damegedCallBack->GetIsDraw())
	{
		//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
		DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
	}

	//���[���[
	m_yoYo->Draw(arg_lightMgr, arg_cam);
}

void Player::EffectDraw(std::weak_ptr<Camera> arg_cam)
{
}

#include"imguiApp.h"
void Player::ImguiDebug()
{
	ImGui::Begin("Player");
	ImGui::Text("Coin : { %d }", m_coinVault.GetNum());
	ImGui::Text("Hp : { %d }", m_hp);
	m_yoYo->AddImguiDebugItem();
	ImGui::End();
}

Vec3<float> Player::GetCenterPos() const
{
	return m_modelObj->m_transform.GetPos();
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
}

void Player::CallBackWithBlock::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
}