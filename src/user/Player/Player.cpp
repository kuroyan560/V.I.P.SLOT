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

void Player::Jump(Vec3<float>* arg_rockOnPos)
{
	m_move.y = 0.0f;
	m_fallSpeed = ConstParameter::Player::JUMP_POWER;
	m_isOnGround = false;
	m_isOnScaffold = false;
}

void Player::OnLanding(bool arg_isGround)
{
	m_fallSpeed = 0.0f;
	m_move.y = 0.0f;
	if (arg_isGround)m_isOnGround = true;
	else m_isOnScaffold = true;
}

Player::Player()
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
	m_hitEffect->Set("resource/user/img/hitEffect.png", 5, { 5,1 }, { 6.0f,6.0f }, 3);

	//�U���R�[���o�b�N
	m_normalAttackCallBack = std::make_shared<PlayersNormalAttack>(&m_ability.m_offensive,&m_coinVault, m_hitEffect, enemyHitSE, enemyKillSE);
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
	m_bodySphereCol = std::make_shared<CollisionSphere>(
		1.4f,
		Vec3<float>(0.0f, -0.2f, 0.0f));

	//�����̓����蔻�苅
	std::shared_ptr<CollisionPrimitive>footSphereCol = std::make_shared<CollisionSphere>(
		1.0f,
		Vec3<float>(0.0f, -1.5f, 0.0f));

	/*--- �R�[���o�b�N���� ---*/
	//�p���B�U���R�[���o�b�N
	m_parryAttackCallBack = std::make_shared<PlayersParryAttack>(&m_ability.m_offensive, arg_objMgr, arg_collisionMgr, parrySE);
	//��_���[�W�R�[���o�b�N
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, arg_cam, onDamagedHitStopSE, onDamagedSE);

	/*--- �R���C�_�[�����i���菇�j ---*/

	std::vector<std::shared_ptr<Collider>>colliders;

	//���f���S�̂𕢂��R���C�_�[
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			m_bodySphereCol
		};
		m_bodyCollider = std::make_shared<Collider>();
		m_bodyCollider->Generate("Player_Body", { "Player" }, coverModelPrimitiveArray);
		m_bodyCollider->SetParentObject(this);
		m_bodyCollider->SetParentTransform(&m_modelObj->m_transform);

		//��_���[�W�R�[���o�b�N�A�^�b�`
		m_bodyCollider->SetCallBack("Enemy", m_damegedCallBack.get());
		m_bodyCollider->SetCallBack("Enemy_Attack", m_damegedCallBack.get());
		colliders.emplace_back(m_bodyCollider);
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

	//�X�^�[�g�ʒu�Ɉړ�
	m_modelObj->m_transform.SetPos(INIT_POS);

	m_oldPos = INIT_POS;

	//�ړ����x
	m_move = { 0,0,0 };

	//�������x������
	m_fallSpeed = 0.0f;

	//�ڒn�t���O������
	m_isOnGround = true;

	//���������Z�b�g
	m_coinVault.Set(0);

	//��_���[�W�R�[���o�b�N
	m_damegedCallBack->Init();

	//���[���[
	m_yoYo->Init();

	//�f�t�H���g�E����
	m_vecX = 1.0f;

	//���ꂩ��~��鏈��������
	m_stepDownTimer.Reset(3);
	m_stepDown = false;

	//�q�b�g�G�t�F�N�g
	m_hitEffect->Init();
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
	m_stepDownTimer.UpdateTimer(timeScale);
	//���ꂩ��~���
	bool stepDownInput = (-moveInput.y < 0.0f);
	if (stepDownInput)
	{
		if (!m_stepDown && m_isOnScaffold)
		{
			m_stepDownTimer.Reset();
			m_stepDown = true;
			m_isOnScaffold = false;
			m_fallSpeed = m_stepDownFallSpeed;
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

	//�����߂��i���j
	if (pos.y < FIELD_FLOOR_TOP_SURFACE_HEIGHT + MODEL_SIZE.y / 2.0f)
	{
		pos.y = FIELD_FLOOR_TOP_SURFACE_HEIGHT + MODEL_SIZE.y / 2.0f;
		OnLanding(true);
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
}

void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (m_damegedCallBack->GetIsDraw())
	{
		BasicDraw::Draw(*arg_lightMgr.lock(), m_modelObj, *arg_cam.lock(), m_drawParam);
	}

	//���[���[
	m_yoYo->Draw(arg_lightMgr, arg_cam, m_drawParam);
}

void Player::Draw2D(std::weak_ptr<Camera> arg_cam)
{
	//�q�b�g�G�t�F�N�g
	m_hitEffect->Draw(arg_cam);

	//HP�`��
	m_playerHp.Draw2D();
}

#include"imguiApp.h"
void Player::ImguiDebug()
{
	ImGui::Begin("Player");
	ImGui::Text("Coin : { %d }", m_coinVault.GetNum());
	ImGui::Text("Offensive : { %d }", m_ability.m_offensive);

	m_drawParam.ImguiDebugItem();
	m_yoYo->AddImguiDebugItem();
	ImGui::End();

	m_playerHp.ImguiDebug();
}

void Player::HitCheckWithScaffold(const std::weak_ptr<Scaffold> arg_scaffold)
{
	//����Ƃ̔����؂��Ă���
	if (!m_stepDownTimer.IsTimeUp())return;

	//���g�̍��W�擾
	auto pos = m_modelObj->m_transform.GetPos();

	//�������Ă��Ȃ�
	if (m_oldPos.y <= pos.y)return;

	//����|�C���^�擾
	auto scaffold = arg_scaffold.lock();

	//�P�t���[���O�̍��W
	auto oldPos = m_oldPos;

	//�����蔻��
	auto scaffoldPos = scaffold->GetPos();
	auto scaffoldNormal = scaffold->GetNormal();

	float myRadius = m_bodySphereCol->m_radius;
	auto v1 = pos + Vec3<float>(0.0f, -myRadius, 0.0f) - scaffoldPos;
	auto v2 = oldPos+ Vec3<float>(0.0f, -myRadius, 0.0f) - scaffoldPos;

	//����ʂƏՓ˂��ĂȂ�
	if (0.0f < v1.Dot(scaffoldNormal) * v2.Dot(scaffoldNormal))return;

	//����ʂ܂ł̋���
	auto d1 = abs(scaffoldNormal.Dot(v1));
	auto d2 = abs(scaffoldNormal.Dot(v2));
	float a = d1 / (d1 + d2);
	auto v3 = v1 * (1 - a) + v2 * a;

	//�Փ˓_
	auto inter = v3 + scaffoldPos;

	//�Փ˓_������ʏ�Ɋ܂܂�Ă��邩
	auto scaffoldWidth = scaffold->GetWidth();
	//����̊e�p
	Vec3<float>p1 = scaffoldPos + Vec3<float>(-scaffoldWidth / 2.0f, 0.0f, 0.5f);
	Vec3<float>p2 = scaffoldPos + Vec3<float>(scaffoldWidth / 2.0f, 0.0f, 0.5f);
	Vec3<float>p3 = scaffoldPos + Vec3<float>(-scaffoldWidth / 2.0f, 0.0f, -0.5f);
	Vec3<float>p4 = scaffoldPos + Vec3<float>(scaffoldWidth / 2.0f, 0.0f, -0.5f);
	
	if ((inter - p1).Cross(p1 - p2).GetNormal() != scaffoldNormal)return;
	if ((inter - p2).Cross(p2 - p4).GetNormal() != scaffoldNormal)return;
	if ((inter - p4).Cross(p4 - p3).GetNormal() != scaffoldNormal)return;
	if ((inter - p3).Cross(p3 - p1).GetNormal() != scaffoldNormal)return;

	//�����߂�
	pos.y = inter.y + myRadius + 0.01f;
	m_modelObj->m_transform.SetPos(pos);

	//���n���̏���
	OnLanding(false);
}

Vec3<float> Player::GetCenterPos() const
{
	return m_modelObj->m_transform.GetPos();
}

bool Player::IsAttack() const
{
	return m_yoYo->IsInvincible();
}