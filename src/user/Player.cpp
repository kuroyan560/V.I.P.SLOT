#include "Player.h"
#include"Object.h"
#include"DrawFunc3D.h"
#include"Collision.h"
#include"Collider.h"
#include"Model.h"
#include"ModelAnimator.h"
#include"UsersInput.h"
#include"ControllerConfig.h"
#include"magic_enum.h"
#include"GameManager.h"
#include"HealAbility.h"
#include"PrecisionAimAbility.h"
#include"ColliderWithFloor.h"

//�C���X�^���X�������P�ɐ������邽�߂̃t���O
bool Player::s_instanced = false;
//�v���C���[����J�����i�v���C���[�����ς�����x�ɁA�J�����𐶐����Ȃ��čςނ悤�Ɂj
std::shared_ptr<PlayerCamera> Player::s_camera;
//�J�����̃L�[
const std::string Player::s_cameraKey = "PlayerCamera";
//HP�Q�[�W�g�摜
std::shared_ptr<TextureBuffer>Player::s_hpGaugeFrameTex;
//HP�Q�[�W�摜
std::shared_ptr<TextureBuffer>Player::s_hpGaugeTex;
//HP�`��ʒu
Vec2<float>Player::s_hpDrawPos = { 239,694 };
//HP�`��X�P�[��
Vec2<float>Player::s_hpDrawExp = { 0.7f,0.55f };

//�A�r���e�B�V�X�e��
std::shared_ptr<AbilitySystem> Player::s_abilitySys;
//����A�r���e�B�i�񕜁j
std::shared_ptr<HealAbility>Player::s_healAbility;
//����A�r���e�B�i�����G�C���j
std::shared_ptr<PrecisionAimAbility>Player::s_precisionAimAbility;

void Player::StaticInit()
{
	//HP�e�N�X�`���ǂݍ���
	s_hpGaugeTex = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/playerHPGauge.png");
	s_hpGaugeFrameTex = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/playerHPFrame.png");

	//�A�r���e�B�V�X�e������
	s_abilitySys = std::make_shared<AbilitySystem>();

	//����A�r���e�B�i�ŏ����珊�����Ă���A�r���e�B�j
	s_healAbility = std::make_shared<HealAbility>();	//��
	s_precisionAimAbility = std::make_shared<PrecisionAimAbility>();	//�����G�C��
	//�A�r���e�B�V�X�e���ɓo�^
	s_abilitySys->Register(s_healAbility);
	s_abilitySys->Register(s_precisionAimAbility);
}

Player::Player() : m_pushBackColliderCallBack(this), m_attack(GetAnimName(ANIM_TYPE::ATTACK))
{
	//�ÓI�����o������
	static bool s_init = false;
	if (!s_init)
	{
		StaticInit();
		s_init = true;
	}

	assert(!s_instanced);
	s_instanced = true;
	m_model = std::make_shared<ModelObject>("resource/user/", "PrePlayer.gltf");

/*--- �R���C�_�[���� ---*/
	//�{��
	auto bodyCol_Sphere = std::make_shared<CollisionSphere>(3.0f, Vec3<float>(0, 6, 0), &m_model->m_transform);
	auto bodyCol = Collider::Generate(
		"Player's Body Collider",
		bodyCol_Sphere,
		&m_pushBackColliderCallBack,	//�����߂��R�[���o�b�N�������A�^�b�`
		COLLIDER_ATTRIBUTE::PLAYER,
		COLLIDER_ATTRIBUTE::ENEMY | COLLIDER_ATTRIBUTE::TERRIAN);
	m_colliders.emplace_back(bodyCol);

	//���Ƃ̓����蔻��p�@�\
	m_colliderWithFloor = std::make_shared<ColliderWithFloor>("Player", Vec3<float>(0, 0, 0), &m_model->m_transform);
	m_colliders.emplace_back(m_colliderWithFloor->GetCollider());

	//�ʏ�U���p�R���C�_�[
	auto nrmAttackCol_Sphere = std::make_shared<CollisionSphere>(3.0f, Vec3<float>(0, 6, 6), &m_model->m_transform);
	auto nrmAttackCol = Collider::Generate(
		"Player's Attack Collider",
		nrmAttackCol_Sphere,
		&m_attack,
		COLLIDER_ATTRIBUTE::PLAYER_ATTACK,
		COLLIDER_ATTRIBUTE::ENEMY);
	m_colliders.emplace_back(nrmAttackCol);

	//�U�������ɕ���R���C�_�[���A�^�b�`
	m_attack.Attach(m_model->m_animator, nrmAttackCol);

	//�A�r���e�B������
	s_precisionAimAbility->Init();
}

void Player::OnStatusTriggerUpdate(const Vec3<float>& InputMoveVec)
{
	//���݂̃X�e�[�^�X
	{
		//�ҋ@
		if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::WAIT))
		{
			//�ҋ@�A�j���[�V����
			m_model->m_animator->speed = 1.0f;
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::WAIT), true, false);
		}
		//�ړ�
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::MOVE))
		{
			//�ړ��A�j���[�V����
			m_model->m_animator->speed = 1.5f;
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::MOVE), true, false);
		}
		//�W�����v
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::JUMP))
		{
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::JUMP), true, false);

			m_colliderWithFloor->Jump(m_jumpPower);
		}
		//�K�[�h
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::GUARD))
		{
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::GUARD), true, false);
		}
		//���
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::DODGE))
		{
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::DODGE), false, false);

			//����t���[��������
			m_dodgeFrame = 0;
			//����̓��͕����L�^
			m_dodgeMoveVec = InputMoveVec;
		}
		//�_�b�V��
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::RUN))
		{
			m_model->m_animator->Play(GetAnimName(ANIM_TYPE::RUN), true, false);
		}
		//�}�[�L���O
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::MARKING))
		{
		}
		//�P�����b�V���`���[�W
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::CHARGE))
		{
			m_singleRushTimer = 0;
		}
		//���b�V��
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::RUSH))
		{
			//�P�����b�V����
			bool isOneShot = m_statusMgr.CompareOldStatus(PLAYER_STATUS_TAG::CHARGE);
			//���b�V�����s
			m_rushSystem.Excute(
				m_markingSystem,
				m_model->m_transform.GetPos(),
				isOneShot,
				static_cast<float>(m_singleRushTimer / m_singleRushChargeTime));
			//�}�[�L���O������
			m_markingSystem.Init();
			//���b�V���p�J�����ɕύX
			if(!isOneShot)GameManager::Instance()->ChangeCamera(RushSystem::s_cameraKey);
		}
		//�A�r���e�B
		else if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::INVOKE_ABILITY))
		{
			s_abilitySys->Invoke();
		}
	}
	//�ߋ��̃X�e�[�^�X
	{
		//���b�V�����I�������u��
		if (m_statusMgr.StatusTrigger(PLAYER_STATUS_TAG::RUSH, true))
		{
			//���b�V���̃J�����擾
			auto rushCam = GameManager::Instance()->GetNowCamera();
			//�v���C���[�p�J�����ɕύX
			GameManager::Instance()->ChangeCamera(Player::s_cameraKey);
			//�v���C���[�J����������
			//s_camera->Init(m_model->m_transform);
			//���b�V���J�����̍Ō�̏�Ԃ��v���C���[�p�J�����ɔ��f������
			s_camera->GetCam()->SetPos(rushCam->GetPos());
			s_camera->GetCam()->SetTarget(rushCam->GetTarget());
			s_camera->LookAtPlayersFront(m_model->m_transform);
			//�\����������
			m_markingSystem.InitPredictionLines();
		}
	}
}

const std::string& Player::GetAnimName(const ANIM_TYPE& Type)
{
	auto& str = m_animName[static_cast<int>(Type)];
	if (str.empty())return m_animName[static_cast<int>(ANIM_TYPE::WAIT)];
	return str;
}

void Player::Init()
{
	//�X�e�[�^�X������
	m_statusMgr.Init(PLAYER_STATUS_TAG::WAIT);

	//�����ʒu�ƌ���
	static Vec3<float>INIT_POS = { 0,0,-5 };
	m_model->m_transform.SetPos(INIT_POS);
	m_model->m_transform.SetRotate(XMMatrixIdentity());

	//�J����������
	s_camera->Init(m_model->m_transform);

	//�ҋ@�A�j���[�V����
	m_model->m_animator->Play(GetAnimName(ANIM_TYPE::WAIT), true, false);

	//���Ƃ̓����蔻��@�\�������i�ŏ��͒n�ɑ������Ă���j
	m_colliderWithFloor->Init();

	//���������
	m_dodgeFrame = m_dodgeFrameNum;

	//�U������������
	m_attack.Init();
	m_oldAttackInput = false;

	//�}�[�L���O�V�X�e��������
	m_markingSystem.Init();
	m_markingTimer = 0;

	//HP������
	m_hp = m_maxHp;

	//�A�r���e�B�V�X�e��������
	s_abilitySys->Init();
}

void Player::Update(UsersInput& Input, ControllerConfig& Controller, const float& Gravity, const Vec2<float>& WinSize)
{
	//�ړ��ʃ��Z�b�g
	Vec3<float>move = { 0,0,0 };

	//HP�񕜃A�r���e�B�X�V
	s_healAbility->Update(m_maxHp, m_hp);
	//�����G�C���X�V
	s_precisionAimAbility->Update();

	//�A�r���e�B�I��
	s_abilitySys->Update(Input, Controller);

	PlayerParameterForStatus infoForStatus;
	infoForStatus.m_markingNum = m_markingSystem.GetSumMarkingCount();
	infoForStatus.m_maxMarking = m_markingSystem.GetSumMarkingCount() == m_maxMarkingCount;
	infoForStatus.m_onGround = m_colliderWithFloor->GetOnGround();
	infoForStatus.m_attackFinish = !m_attack.IsActive();
	infoForStatus.m_dodgeFinish = (m_dodgeFrameNum <= m_dodgeFrame);
	infoForStatus.m_rushFinish = !m_rushSystem.IsActive();
	infoForStatus.m_abilityFinish = s_abilitySys->IsNowAbilityFinish();

	//�X�e�[�^�X�̍X�V
	m_statusMgr.Update(Input, Controller, infoForStatus);

	//���X�e�B�b�N���̓��[�g
	auto stickL = Controller.GetMoveVec(Input);

	//�ړ�����
	Vec3<float>inputMoveVec = { stickL.x,0.0f,-stickL.y };

	//�J�����ʒu�p�x�̃I�t�Z�b�g����X�e�B�b�N�̓��͕����␳
	static const Angle ANGLE_OFFSET(-90);
	inputMoveVec = KuroMath::TransformVec3(inputMoveVec, KuroMath::RotateMat({ 0,1,0 }, -s_camera->GetPosAngle() + ANGLE_OFFSET)).GetNormal();

	//�X�e�[�^�X�̃g���K�[�����m���āA�������Ăяo��
	OnStatusTriggerUpdate(inputMoveVec);

	//�A���U���̓���
	bool attackInput = Controller.GetHandleInput(Input, HANDLE_INPUT_TAG::ATTACK);
	//�U�����
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::ATTACK))
	{
		if (!m_oldAttackInput && attackInput)
		{
			//�U���̏�����PlayerAttack���ŏ���
			m_attack.Attack(m_model->m_transform.GetFront());
		}
	}
	//�g���K�[����p�ɉߋ��̓��͂Ƃ��ċL�^
	m_oldAttackInput = attackInput;

	//�ʏ�ړ����
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::MOVE))
	{
		//���͂ɂ��ړ��̏���
		move += inputMoveVec * m_moveSpeed;
	}
	//����ړ����
	else if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::RUN))
	{
		//���͂ɂ��ړ��̏���
		move += inputMoveVec * m_runSpeed;
	}
	//������
	else if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::DODGE))
	{
		//��𑬓x�v�Z
		float dodgeSpeed = m_dodgeEasingParameter.Calculate(
			static_cast<float>(m_dodgeFrame), 
			static_cast<float>(m_dodgeFrameNum), 
			m_dodgePower,
			m_runSpeed);
		//����ɂ��ړ��ʉ��Z
		move += m_dodgeMoveVec * dodgeSpeed;
		//����t���[���v��
		m_dodgeFrame++;
	}

	//�}�[�L���O���
	bool nowMarkingInput = false;
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::MARKING))
	{
		//���͂ɂ��ړ��̏���
		move += inputMoveVec * m_whileMarkingMoveSpeed;

		//�}�[�L���O�������̃C���^�[�o��
		if (m_markingTimer)
		{
			m_markingTimer--;
		}
		else
		{
			//�}�[�L���O���s
			m_markingTimer = m_markingSpan;
			nowMarkingInput = true;
		}
	}
	else m_markingTimer = 0;

	//���b�V�����
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::RUSH))
	{
		m_rushSystem.Update(m_model->m_transform, Gravity);
	}

	//�P�����b�V���̃`���[�W���
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::CHARGE))
	{
		//���͂ɂ��ړ��̏���
		move += inputMoveVec * m_whileChargeMoveSpeed;
		//�`���[�W���Ԓ~�ρi����𒴂��Ȃ��悤�ɂ���j
		if (m_singleRushChargeTime < ++m_singleRushTimer)m_singleRushTimer = m_singleRushChargeTime;
	}

	//�������ԂłȂ��Ƃ�
	if (!m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::OUT_OF_CONTROL))
	{
		//���b�N�I��
		if (Controller.GetCameraRock(Input))s_camera->RockOn(m_model->m_transform);

		//�v���C���[�Ǐ]�J�����X�V
		s_camera->Update(m_model->m_transform, Controller.GetCameraVec(Input));
	}

	//�U�������X�V
	m_attack.Update();
	//�U���X�e�[�^�X�łȂ��i�U�������f���ꂽ�ꍇ�j
	if (!m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::ATTACK))m_attack.Stop();

	//�A�j���[�V�����X�V
	m_model->m_animator->Update();

	//�U���̐������ړ��ʂɉ��Z
	move += m_attack.GetMomentum();

	//����
	move.y += m_colliderWithFloor->GetFallSpeed();
	m_colliderWithFloor->AddFallSpeed(Gravity);

	//���b�V����
	if (m_statusMgr.CompareNowStatus(PLAYER_STATUS_TAG::RUSH))
	{
		//�ړ��ʑł�����
		move = { 0,0,0 };
		//�������x���Z�b�g
		m_colliderWithFloor->ResetFallSpeed();
	}

	//�ړ��ʂ𔽉f������
	auto playerPos = m_model->m_transform.GetPos();
	playerPos += move;
	m_model->m_transform.SetPos(playerPos);

	//�}�[�L���O�V�X�e���X�V
	m_markingSystem.Update(*s_camera->GetCam(), WinSize, nowMarkingInput, playerPos, m_maxMarkingCount);

	//�����]���iXZ���ʁj
	if (move.x || move.z)
	{
		m_model->m_transform.SetLookAtRotate(playerPos + Vec3<float>(move.x, 0.0f, move.z));
	}
}

#include"BasicDraw.h"
void Player::Draw(Camera& Cam, LightManager& LigManager, std::shared_ptr<CubeMap>AttachCubeMap)
{
	//�v���C���[�`��
	BasicDraw::Draw(LigManager, m_model, Cam, AttachCubeMap);

	//�}�[�L���O�\�����`��
	m_markingSystem.DrawPredictionLine(Cam, m_model->m_transform.GetPos());
}

#include"DrawFunc2D_Color.h"
#include"DrawFunc2D.h"
void Player::DrawHUD(Camera& Cam, const Vec2<float>& WinSize)
{
	s_camera->Draw(Cam);

	//���b�V���V�X�e�����̓}�[�L���OHUD��\��
	if (!m_rushSystem.IsActive())
	{
		m_markingSystem.DrawHUD(Cam, WinSize);
	}

	//HP�g�`��
	DrawFunc2D::DrawRotaGraph2D(
		s_hpDrawPos,
		s_hpDrawExp,
		0.0f,
		s_hpGaugeFrameTex
	);

	//HP�Q�[�W�`��
	float hpRate = (float)m_hp / m_maxHp;
	DrawFunc2D_Color::DrawRotaGraph2D(
		s_hpDrawPos,
		s_hpDrawExp,
		0.0f,
		s_hpGaugeTex,
		Color(0, 0, 0, 0),
		{ 0.5f,0.5f },
		{ false,false },
		{ hpRate,0.0f },
		{ 1.0f,1.0f }
	);

	//�A�r���e�B�V�X�e��
	s_abilitySys->DrawHUD();

	//�f�o�b�O�p
	s_healAbility->Draw();
	s_precisionAimAbility->Draw();
}

#include"imguiApp.h"
void Player::ImguiDebug()
{
	ImGui::Begin("Player");
/*--- �p�����[�^�\�� ---*/
	//�ő�HP
	if (ImGui::DragInt("MaxHp", &m_maxHp))
	{
		if (m_maxHp <= 0)m_maxHp = 1;		//�O�ȉ��h�~
		if (m_maxHp < m_hp)m_hp = m_maxHp;	//���݂�HP�̍ő咴�ߖh�~

	}
	//���݂�HP�𒼐ړ���
	int oldHp = m_hp;
	if (ImGui::DragInt("NowHp", &m_hp))
	{
		//�}�C�i�X�h�~
		if (m_hp < 0)m_hp = 0;
		//�ő�HP�𒴂��Ȃ�
		if (m_maxHp < m_hp)m_hp = m_maxHp;
	}
	//HP�`��ʒu
	static float s_imguiHpDrawPos[2] = { s_hpDrawPos.x,s_hpDrawPos.y };
	if (ImGui::DragFloat2("HpDrawPos", s_imguiHpDrawPos))
	{
		s_hpDrawPos = { s_imguiHpDrawPos[0],s_imguiHpDrawPos[1] };
	}
	//HP�`��X�P�[��
	static float s_imguiHpDrawExp[2] = { s_hpDrawExp.x,s_hpDrawExp.y };
	if (ImGui::DragFloat2("HpDrawExp", s_imguiHpDrawExp, 0.05))
	{
		s_hpDrawExp = { s_imguiHpDrawExp[0],s_imguiHpDrawExp[1] };
	}

	ImGui::Separator();
/*--- �X�e�[�^�X�\�� ---*/
	//�X�e�[�^�X���擾
	static std::string s_nowStatusName = std::string(magic_enum::enum_name(m_statusMgr.GetNowStatus()));
	static std::string s_beforeStatusName = s_nowStatusName;

	//�X�e�[�^�X�؂�ւ��g���K�[����擾
	if (m_statusMgr.StatusTrigger())
	{
		s_beforeStatusName = s_nowStatusName;
		s_nowStatusName = std::string(magic_enum::enum_name(m_statusMgr.GetNowStatus()));
	}

	//�\��
	ImGui::Text("NowStatus : { %s }", s_nowStatusName.c_str());
	ImGui::Text("BeforeStatus : { %s }", s_beforeStatusName.c_str());

/*--- ���\���� ---*/
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(500, 320), ImGuiWindowFlags_NoTitleBar);
	//�ʏ�ړ�
	if (ImGui::TreeNode("Move"))
	{
		if (ImGui::DragFloat("Speed", &m_moveSpeed, 0.1f) && m_moveSpeed < 0)m_moveSpeed = 0.0f;	//�}�C�i�X�h�~
		ImGui::TreePop();
	}
	//����ړ�
	if (ImGui::TreeNode("Run"))
	{
		if (ImGui::DragFloat("Speed", &m_runSpeed, 0.1f) && m_runSpeed < 0)m_runSpeed = 0.0f;	//�}�C�i�X�h�~
		ImGui::TreePop();
	}
	//�W�����v��
	if (ImGui::TreeNode("Jump"))
	{
		if (ImGui::DragFloat("Power", &m_jumpPower, 0.1f) && m_jumpPower < 0)m_jumpPower = 0.0f;	//�}�C�i�X�h�~
		ImGui::TreePop();
	}
	//�ʏ�U��
	if (ImGui::TreeNode("Attack"))
	{
		m_attack.ImguiDebug();
		ImGui::TreePop();
	}

	//���
	if (ImGui::TreeNode("Dodge"))
	{
		//����ɂ�����t���[����
		if (ImGui::DragInt("FrameNum", &m_dodgeFrameNum) && m_dodgeFrameNum < 0)m_dodgeFrameNum = 0;	//�}�C�i�X�h�~
		//����
		if (ImGui::DragFloat("Power", &m_dodgePower, 0.1f) && m_dodgePower < 0)m_dodgePower = 0;	//�}�C�i�X�h�~

		//����̃C�[�W���O�ω��iIn,Out,InOut)
		//���O�\��
		ImGui::Text("%s", magic_enum::enum_name(m_dodgeEasingParameter.m_changeType).data());
		//�ύX
		static int s_dodgeSelectEaseChange = m_dodgeEasingParameter.m_changeType;
		if (ImGui::SliderInt("EaseChangeType", &s_dodgeSelectEaseChange, 0, EASE_CHANGE_TYPE_NUM - 1))
		{
			m_dodgeEasingParameter.m_changeType = (EASE_CHANGE_TYPE)s_dodgeSelectEaseChange;
		}

		//����̃C�[�W���O�^�C�v
		//���O�\��
		ImGui::Text("%s", magic_enum::enum_name(m_dodgeEasingParameter.m_easeType).data());
		//�ύX
		static int s_dodgeSelectEase = m_dodgeEasingParameter.m_easeType;
		if (ImGui::SliderInt("EaseType", &s_dodgeSelectEase, 0, EASING_TYPE_NUM - 1))
		{
			m_dodgeEasingParameter.m_easeType = (EASING_TYPE)s_dodgeSelectEase;
		}

		ImGui::TreePop();
	}
	//�}�[�L���O
	if (ImGui::TreeNode("Marking"))
	{
		//�}�[�L���O���������̎��Ԍv���p�^�C�}�[
		ImGui::Text("MarkingTimer : { %d }", m_markingTimer);

		//���݂̑��}�[�L���O��
		ImGui::Text("SumMarkingCount : { %d }", m_markingSystem.GetSumMarkingCount());

		//�ő�}�[�L���O��
		if (ImGui::DragInt("MaxMarkingCount", &m_maxMarkingCount) && m_maxMarkingCount < 0)m_maxMarkingCount = 0;		//�}�C�i�X�h�~

		//�}�[�L���O�������X�p��
		if (ImGui::DragInt("MarkingSpan", &m_markingSpan) && m_markingSpan < 0)m_markingSpan = 0;		//�}�C�i�X�h�~

		//�}�[�L���O���̈ړ����x
		if (ImGui::DragFloat("MoveSpeed", &m_whileMarkingMoveSpeed,0.1f) && m_whileMarkingMoveSpeed < 0.0f)m_whileMarkingMoveSpeed = 0.0f;		//�}�C�i�X�h�~

		//�}�[�L���O�V�X�e���̏������{�^��
		if (ImGui::Button("Init - MarkingSystem"))m_markingSystem.Init();

		ImGui::TreePop();
	}
	//���b�V��
	if (ImGui::TreeNode("Rush"))
	{
		//�P�����b�V���̃`���[�W���̈ړ����x
		if (ImGui::DragFloat("MoveSpeed_oneShotCharge", &m_whileChargeMoveSpeed, 0.1f) && m_whileChargeMoveSpeed < 0.0f)m_whileChargeMoveSpeed = 0.0f;		//�}�C�i�X�h�~

		ImGui::Separator();
		m_rushSystem.ImguiDebug();

		ImGui::TreePop();
	}
	//�P�����b�V��
	if (ImGui::TreeNode("SingleRush"))
	{
		if (ImGui::DragInt("ChargeTotalTime", &m_singleRushChargeTime) && m_singleRushChargeTime < 0)m_singleRushChargeTime = 0;		//�}�C�i�X�h�~
		ImGui::TreePop();
	}

	//�A�r���e�B
	if (ImGui::TreeNode("Ability"))
	{
		s_abilitySys->ImguiDebug();
		ImGui::TreePop();
	}

	ImGui::EndChild();

	ImGui::End();
}

void Player::PushBackColliderCallBack::OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)
{
	//���݂̍��W
	Vec3<float>nowPos = m_parent->m_model->m_transform.GetPos();

	//�����߂���̍��W�i�[��
	Vec3<float>pushBackPos = nowPos;

	//���g�̃R���C�_�[�v���~�e�B�u�i���j���擾
	auto mySphere = (CollisionSphere*)GetAttachCollider().lock()->GetColliderPrimitive().lock().get();

	//����̃R���C�_�[�v���~�e�B�u���擾
	auto otherPrimitive = OtherCollider.lock()->GetColliderPrimitive().lock();
	if (otherPrimitive->GetShape() == CollisionPrimitive::SPHERE)
	{
		//����̏Փ˔���p���擾
		CollisionSphere* otherSphere = (CollisionSphere*)otherPrimitive.get();

		//���ꂼ��̒��S���W
		Vec3<float>myCenter = mySphere->GetCenter();
		Vec3<float>otherCenter = otherSphere->GetCenter();

		//�����߂�����
		Vec3<float>pushBackVec = (myCenter - otherCenter).GetNormal();

		//�����߂���
		float pushBackAmount = mySphere->m_radius + otherSphere->m_radius + 0.1f;
		
		//�����߂�����̃R���C�_�[�̍��W
		Vec3<float>pushBackColPos = otherCenter + pushBackVec * pushBackAmount;

		//�����߂���̃R���C�_�[�ƌ��݂̃R���C�_�[���W�Ƃ̃I�t�Z�b�g
		Vec3<float>colOffset = pushBackColPos - myCenter;

		//�I�t�Z�b�g��e�ł���g�����X�t�H�[���ɓK�p
		pushBackPos = nowPos + colOffset;
	}
	else assert(0);	//�p�ӂ��Ă��Ȃ�

	//�V�������W���Z�b�g
	m_parent->m_model->m_transform.SetPos(pushBackPos);
}

const Transform& Player::GetTransform()const
{
	return m_model->m_transform;
}
