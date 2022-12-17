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
#include"Scaffold.h"
#include"TexHitEffect.h"
#include"BasicDraw.h"
#include"PlayersCounterAttackHitEffect.h"

void Player::OnAir()
{
	m_isOnGround = false;
	m_isOnScaffold = false;

	//�G���݂��R���C�_�[�I��
	m_footSphereCollider->SetActive(true);
}

void Player::Jump()
{
	m_move.y = 0.0f;
	m_fallSpeed = ConstParameter::Player::JUMP_POWER;
	OnAir();
}

void Player::OnLanding(bool arg_isGround)
{
	m_fallSpeed = 0.0f;
	m_move.y = 0.0f;
	if (arg_isGround)m_isOnGround = true;
	else m_isOnScaffold = true;

	//�G���݂��R���C�_�[�I�t
	m_footSphereCollider->SetActive(false);
}

void Player::OnImguiItems()
{
	ImGui::Text("Coin : { %d }", m_coinVault.GetNum());
	ImGui::Text("Offensive : { %d }", m_ability.m_offensive);

	m_drawParam.ImguiDebugItem();
	m_yoYo->AddImguiDebugItem();
}

void Player::InitMovement()
{
	using namespace ConstParameter::Player;

	//�X�^�[�g�ʒu�Ɉړ�
	m_modelObj->m_transform.SetPos(INIT_POS);

	m_oldPos = INIT_POS;

	//�ړ����x
	m_move = { 0,0,0 };

	//�������x������
	m_fallSpeed = 0.0f;

	//�ڒn�t���O������
	m_isOnGround = true;

	//���[���[
	m_yoYo->Init();

	//�f�t�H���g�E����
	m_vecX = 1.0f;

	//���ꂩ��~��鏈��������
	m_stepDown = false;
}

Player::Player() : Debugger("Player")
{
	//���f���ǂݍ���
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	std::string soundDir = "resource/user/sound/";

	//�W�����vSE�ǂݍ���
	m_jumpSE = AudioApp::Instance()->LoadAudio(soundDir + "player_jump.wav", 0.5f);

	//�G�ւ̍U��SE
	int enemyHitSE = AudioApp::Instance()->LoadAudio(soundDir + "enemy_damage.wav", 0.5f);

	//�G��|�����Ƃ���SE
	int enemyKillSE = AudioApp::Instance()->LoadAudio(soundDir + "enemy_dead.wav", 0.5f);

	//�q�b�g�G�t�F�N�g����
	m_hitEffect = std::make_shared<TexHitEffect>();
	m_hitEffect->Set("resource/user/img/hitEffect.png", 5, { 5,1 }, { 6.0f,6.0f }, 3, false);
	m_counterHitEffect = std::make_shared<PlayersCounterAttackHitEffect>();

	//�U���R�[���o�b�N
	m_normalAttackCallBack = std::make_shared<PlayersNormalAttack>(&m_ability.m_offensive,&m_coinVault, m_hitEffect, enemyHitSE, enemyKillSE);
	//���ˍU���R�[���o�b�N
	m_counterAttackCallBack = std::make_shared<PlayersCounterAttack>(&m_ability.m_offensive, &m_coinVault, m_counterHitEffect.get(), enemyHitSE, enemyKillSE);
}

void Player::Awake(std::weak_ptr<CollisionManager> arg_collisionMgr, std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<GameCamera> arg_cam)
{
	using namespace ConstParameter::Player;

	std::string soundDir = "resource/user/sound/";

	//��_���[�W�q�b�g�X�g�b�vSE
	int onDamagedHitStopSE = AudioApp::Instance()->LoadAudio(soundDir + "player_damage_hitStop.wav", 0.5f);

	//��_���[�WSE
	int onDamagedSE = AudioApp::Instance()->LoadAudio(soundDir + "player_damage_onTrigger.wav", 0.4f);

	//�p���[SE
	int parrySE = AudioApp::Instance()->LoadAudio(soundDir + "parry.wav", 0.8f);

	/*--- �R���C�_�[�p�v���~�e�B�u���� ---*/
	//���f���S�̂𕢂���
	m_bodySphereColPrim = std::make_shared<CollisionSphere>(
		1.4f,
		Vec3<float>(0.0f, -0.2f, 0.0f));

	//���f���S�̂𕢂�AABB
	m_bodyAABBColPrim = std::make_shared<CollisionAABB>(
		m_modelObj->m_model->GetAllMeshPosMinMax());

	//�����̓����蔻�苅
	m_footSphereColPrim = std::make_shared<CollisionSphere>(
		1.0f,
		Vec3<float>(0.0f, -1.5f, 0.0f));

	/*--- �R�[���o�b�N���� ---*/
	//�p���B�U���R�[���o�b�N
	m_parryAttackCallBack = std::make_shared<PlayersParryAttack>(&m_ability.m_offensive, arg_objMgr, arg_collisionMgr, parrySE);
	//��_���[�W�R�[���o�b�N
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, arg_cam, onDamagedHitStopSE, onDamagedSE);
	//�����߂�
	m_pushBackCallBack = std::make_shared<PushBackCallBack>(this);
	//�񕜃L�b�g���
	m_getHealKitCallBack = std::make_shared<GetHealKitCallBack>(this);
	//���݂��W�����v
	m_stepCallBack = std::make_shared<StepCallBack>(this);

	/*--- �R���C�_�[�����i���菇�j ---*/

	std::vector<std::shared_ptr<Collider>>colliders;

	//���f���S�̂𕢂����R���C�_�[
	{
		m_bodySphereCollider = std::make_shared<Collider>();
		m_bodySphereCollider->Generate("Player_Body_Sphere", { "Player" }, { m_bodySphereColPrim });
		m_bodySphereCollider->SetParentObject(this);
		m_bodySphereCollider->SetParentTransform(&m_modelObj->m_transform);

		//��_���[�W�R�[���o�b�N�A�^�b�`
		m_bodySphereCollider->SetCallBack("Enemy", m_damegedCallBack.get());
		m_bodySphereCollider->SetCallBack("Enemy_Attack", m_damegedCallBack.get());

		//�񕜃L�b�g����R�[���o�b�N�A�^�b�`
		m_bodySphereCollider->SetCallBack("Heal_Kit", m_getHealKitCallBack.get());

		colliders.emplace_back(m_bodySphereCollider);
	}
	//���f���S�̂𕢂�AABB�R���C�_�[
	{
		m_bodyAABBCollider = std::make_shared<Collider>();
		m_bodyAABBCollider->Generate("Player_Body_AABB", { "Player" }, { m_bodyAABBColPrim });
		m_bodyAABBCollider->SetParentObject(this);
		m_bodyAABBCollider->SetParentTransform(&m_modelObj->m_transform);

		//�����߂��R�[���o�b�N�A�^�b�`
		m_bodyAABBCollider->SetCallBack("Scaffold", m_pushBackCallBack.get());
		m_bodyAABBCollider->SetCallBack("Floor", m_pushBackCallBack.get());

		colliders.emplace_back(m_bodyAABBCollider);
	}
	//���f���̑����̋��R���C�_�[
	{
		m_footSphereCollider = std::make_shared<Collider>();
		m_footSphereCollider->Generate("Player_Foot_Sphere", { "Player" }, { m_footSphereColPrim });
		m_footSphereCollider->SetParentObject(this);
		m_footSphereCollider->SetParentTransform(&m_modelObj->m_transform);

		//���݂��W�����v�R�[���o�b�N
		m_footSphereCollider->SetCallBack("Enemy", m_stepCallBack.get());

		colliders.emplace_back(m_footSphereCollider);
	}

	/*--- �R���C�_�[�z��o�^ ---*/
	arg_collisionMgr.lock()->Register(colliders);

	/*--- ���[���[���� ---*/
	m_yoYo = std::make_shared<YoYo>(
		arg_collisionMgr,
		&m_modelObj->m_transform,
		m_normalAttackCallBack,
		m_parryAttackCallBack);
	m_yoYo->Awake(3.0f, 2.5f);
}

void Player::Init(PlayersAbility arg_ability, int arg_initRemainLife, int arg_initCoinNum)
{
	using namespace ConstParameter::Player;

	//���݂̔\�͒l���X�V
	m_ability = arg_ability;

	//HP������
	m_playerHp.Init(m_ability.m_maxLife, arg_initRemainLife);

	//���������Z�b�g
	m_coinVault.Set(arg_initCoinNum);

	//���������Z�b�g
	m_coinVault.Set(0);

	//��_���[�W�R�[���o�b�N
	m_damegedCallBack->Init();

	InitMovement();

	//�q�b�g�G�t�F�N�g
	m_hitEffect->Init();
	m_counterHitEffect->Init();

	//�������R�C��UI
	m_coinUI.Init(m_coinVault.GetNum());
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	using namespace ConstParameter::Environment;

	//�^�C���X�P�[���擾
	const float& timeScale = arg_timeScale.GetTimeScale();

	//���W�擾
	Vec3<float>pos = m_modelObj->m_transform.GetPos();
	m_oldPos = pos;

//���͏��擾
	const auto& input = *UsersInput::Instance();

	//�ړ���������
	Vec2<float>moveInput = { 0,0 };

	//�W�����v�̃g���K�[����
	bool jumpTrigger = false;

	//���[���[��������
	bool throwYoyoTrigger = false;

	//N�U������
	bool neutralTrigger = false;

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
		//N�U������
		neutralTrigger = input.KeyOnTrigger(DIK_LSHIFT);
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
		//N�U������
		neutralTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::RB);
		break;
	}

	//���̑��̓��͐ݒ�ɂȂ��Ă����ꍇ�G���[
	default:
		assert(0);
		break;
	}

	//�����L�^
	if (!moveInput.IsZero())m_vecX = moveInput.x;

//���͏������ɑ���
	float moveSpeed = m_yoYo->IsThrow() ? MOVE_SPEED_WHILE_THROW : MOVE_SPEED;

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
	bool canJump = (m_isOnGround || m_isOnScaffold) && !m_yoYo->IsThrow();
	if (jumpTrigger && canJump)
	{
		Jump();
		AudioApp::Instance()->PlayWaveDelay(m_jumpSE, 3);
	}

	//����Ƃ̓����蔻���؂鎞�ԍX�V
	m_pushBackCallBack->Update(timeScale);
	//���ꂩ��~���
	bool stepDownInput = (-moveInput.y < 0.0f);
	if (stepDownInput)
	{
		if (!m_stepDown && m_isOnScaffold)
		{
			m_pushBackCallBack->NotPushBackWithScaffold(3.0f);
			m_stepDown = true;
			m_fallSpeed = m_stepDownFallSpeed;
			OnAir();
		}
	}
	//���͂��������Ȃ��Ƒ���~������s���Ȃ�
	else
	{
		m_stepDown = false;
	}
	
	//�U������
	m_move.x += m_yoYo->GetAccelX() * timeScale;
	m_fallSpeed += m_yoYo->GetAccelY() * timeScale;

	//�����i�W�����v���Ɨ������ŏd�͕ω��A�f�����W�����v �� �������~���j
	m_move.y += m_fallSpeed * timeScale;

	if (0.0f < m_fallSpeed)
	{
		m_fallSpeed -= GRAVITY_WHILE_JUMP * timeScale;
	}
	else
	{
		if (m_yoYo->IsAir())
		{
			m_fallSpeed -= GRAVITY_WHILE_ATTACK * timeScale;
		}
		else
		{
			m_fallSpeed -= GRAVITY_WHILE_FALL * timeScale;
		}
	}

	//�������x����
	if (m_fallSpeed < FALL_SPEED_MIN)m_fallSpeed = FALL_SPEED_MIN;

	//�ړ��ʉ��Z
	pos += m_move * timeScale;

	//�X�V�������W�̔��f
	m_modelObj->m_transform.SetPos(pos);

	//�����_���[�W����
	if (pos.y < FALL_LIMIT_HEIGHT)
	{
		m_damegedCallBack->Execute(true);
		InitMovement();
	}

	//��_���[�W�R�[���o�b�N
	m_damegedCallBack->Update(arg_timeScale);

//���[���[�i�U���j
	//������
	if (throwYoyoTrigger && m_yoYo->Throw(m_vecX))
	{
		m_fallSpeed = 0.0f;
		m_move.y = 0.0f;
	}
	//N�U��
	if (neutralTrigger && m_yoYo->Neutral())
	{

	}

	//���[���[
	m_yoYo->Update(arg_timeScale, m_vecX);

	//�q�b�g�G�t�F�N�g
	m_hitEffect->Update(timeScale);
	m_counterHitEffect->Update(timeScale);

	//�������R�C��UI
	m_coinUI.Update(m_coinVault.GetNum(), timeScale);

	//�v���C���[HPUI
	m_playerHp.Update(timeScale);

	//���ꂪ�Ȃ�
	if (!m_bodyAABBCollider->GetIsHit())
	{
		//�󒆏�Ԃ�
		OnAir();
	}
}

void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (m_damegedCallBack->GetIsDraw())
	{
		BasicDraw::Instance()->Draw(*arg_lightMgr.lock(), m_modelObj, *arg_cam.lock(), m_drawParam);
	}

	//���[���[
	m_yoYo->Draw(arg_lightMgr, arg_cam, m_drawParam);
}

void Player::Draw2D(std::weak_ptr<Camera> arg_cam)
{
	//�q�b�g�G�t�F�N�g
	m_hitEffect->Draw(arg_cam);
	m_counterHitEffect->Draw(arg_cam);

	//HP�`��
	m_playerHp.Draw2D();

	//�������R�C��UI
	m_coinUI.Draw2D();
}

Vec3<float> Player::GetCenterPos() const
{
	return m_modelObj->m_transform.GetPos();
}

void Player::SetDamageColldierActive(bool arg_active)
{
	m_bodySphereCollider->SetActive(arg_active);
}

bool Player::IsAttack() const
{
	return m_yoYo->IsInvincible();
}
