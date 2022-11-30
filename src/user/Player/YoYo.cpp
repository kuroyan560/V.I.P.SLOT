#include "YoYo.h"
#include"Collision.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"TimeScale.h"
#include<magic_enum.h>
#include"Importer.h"
#include"Object.h"
#include"GameObject.h"
#include"TexHitEffect.h"
#include"AudioApp.h"
#include"ObjectManager.h"

bool YoYo::IsAttackGapInterval(PREVIOUS_INPUT arg_input)
{
	//�C���^�[�o�����łȂ�
	if (m_attackGapInterval.IsTimeUp())return false;

	//�C���^�[�o�����ŁA�\����͉\�Ȃ���͂��L�^
	if (m_attackGapInterval.GetLeftTime() <= static_cast<float>(m_canAttackGapPreviousInputLeftTime))
	{
		m_attackGapPreviousInput = arg_input;
	}

	return true;
}

void YoYo::StatusInit(Vec3<float>arg_accelVec)
{
	//�X�e�[�^�X�̃p�����[�^�擾
	auto& param = m_statusParams[(int)m_status];

	//�A�j���[�V�����Đ�
	if (!param.m_animName.empty())
	{
		m_modelObj->m_animator->Play(
			param.m_animName, false, false);
	}

	//�^�C�}�[���Z�b�g
	m_timer.Reset(param.m_finishInterval);

	//�U���̌��ƂȂ�C���^�[�o���^�C�}�[���Z�b�g
	m_attackGapInterval.Reset(param.m_attackGapInterval);

	//�����x������
	m_iniAccel = param.m_maxAccelVal * arg_accelVec;
	m_accel = m_iniAccel;
}

YoYo::YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr,
	Transform* arg_playerTransform,
	std::weak_ptr< CollisionCallBack>arg_attackCallBack,
	std::weak_ptr< CollisionCallBack>arg_parryCallBack)
{
	//���f���I�u�W�F�N�g����
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "yoyo.glb");

	//�R���C�_�[����
	m_neutralCol = std::make_shared<Collider>();
	m_throwCol = std::make_shared<Collider>();

	/*
		[�g�����X�t�H�[���e�q�֌W]
		�v���C���[ �� �U�������w�� �� ���[���[ �� �e�{�[��
	*/
	m_vecTransform.SetParent(arg_playerTransform);
	m_modelObj->m_transform.SetParent(&m_vecTransform);
	m_modelObj->m_animator->SetParentTransform(m_modelObj->m_transform);

	/*
		�EN�U���̓����蔻��e�F���[���[
		�E�����p�̓����蔻��e�F�e�{�[��
	*/
	m_neutralCol->SetParentTransform(&m_modelObj->m_transform);
	m_throwCol->SetParentTransform(&m_modelObj->m_animator->GetBoneTransform("Bone"));

	//�U���R�[���o�b�N
	m_neutralCol->SetCallBack("Enemy", arg_attackCallBack.lock().get());
	m_neutralCol->SetCallBack("Enemy_Attack", arg_parryCallBack.lock().get());
	m_throwCol->SetCallBack("Enemy", arg_attackCallBack.lock().get());

	//�R���C�_�[�o�^
	arg_collisionMgr.lock()->Register(m_neutralCol);
	arg_collisionMgr.lock()->Register(m_throwCol);

//�X�e�[�^�X���Ƃ̃p�����[�^�ݒ�
	//N�U��
	m_statusParams[NEUTRAL].m_animName = "Attack_N";
	m_statusParams[NEUTRAL].m_finishInterval = 10;
	m_statusParams[NEUTRAL].m_interruptInterval = m_statusParams[NEUTRAL].m_finishInterval;
	m_statusParams[NEUTRAL].m_attackGapInterval = 30;

	//�U��0
	m_statusParams[THROW_0].m_animName = "Attack_0";
	m_statusParams[THROW_0].m_finishInterval = 32;
	m_statusParams[THROW_0].m_interruptInterval = 18;
	m_statusParams[THROW_0].m_maxAccelVal = { 0.2f,0.0f,0.0f };

	//�U��1
	m_statusParams[THROW_1].m_animName = "Attack_1";
	m_statusParams[THROW_1].m_finishInterval = 37;
	m_statusParams[THROW_1].m_interruptInterval = 20;
	m_statusParams[THROW_1].m_maxAccelVal = { 0.3f,0.0f,0.0f };

	//�U��2
	m_statusParams[THROW_2].m_animName = "Attack_2";
	m_statusParams[THROW_2].m_finishInterval = 33;
	m_statusParams[THROW_2].m_interruptInterval = 33;
	m_statusParams[THROW_2].m_maxAccelVal = { 0.6f,0.22f,0.0f };
	m_statusParams[THROW_2].m_attackGapInterval = 10;
}


void YoYo::Awake(float arg_neutralColSphereRadius, float arg_throwColSphereRadius)
{
	static const std::string TAG = "Player_Attack";
	std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;

	//N�U���p�R���C�_�[����
	m_neutralColSphere = std::make_shared<CollisionSphere>(arg_neutralColSphereRadius, Vec3<float>(0, 0, 0));
	colPrimitiveArray = { m_neutralColSphere };
	m_neutralCol->Generate("YoYo - Neutral", { "Player_Attack","Player_Parry" }, colPrimitiveArray);

	//�����p�R���C�_�[����
	m_throwColSphere = std::make_shared<CollisionSphere>(arg_throwColSphereRadius, Vec3<float>(0, 0, 0));
	colPrimitiveArray = { m_throwColSphere };
	m_throwCol->Generate("YoYo - Throw", { "Player_Attack" }, colPrimitiveArray);
}

void YoYo::Init()
{
	//�A�j���[�V����������
	m_modelObj->m_animator->Reset();

	//�R���C�_�[�I�t
	m_neutralCol->SetActive(false);
	m_throwCol->SetActive(false);

	//��Ɏ����Ă���
	m_status = HAND;

	//��s���̓t���O���Z�b�g
	m_tripleAttackPreviousInput = false;

	//�����x���Z�b�g
	m_accel = { 0,0,0 };
	m_iniAccel = { 0,0,0 };

	//�U���̌����Z�b�g
	m_attackGapInterval.Reset(0);
	//�C���^�[�o���p�̗\����̓��Z�b�g
	m_attackGapPreviousInput = NONE_INPUT;
}

void YoYo::Update(const TimeScale& arg_timeScale, float arg_playersVecX)
{
	//�^�C���X�P�[���̎擾
	const float timeScale = arg_timeScale.GetTimeScale();

	//�A�j���[�V�����X�V
	m_modelObj->m_animator->Update(timeScale);

	//�U���̌��C���^�[�o���X�V
	if (m_attackGapInterval.UpdateTimer(timeScale))
	{
		//�\����͊m�F
		if (m_attackGapPreviousInput == THROW_INPUT)Throw(arg_playersVecX);
		else if (m_attackGapPreviousInput == NEUTRAL_INPUT)Neutral();

		//�\����̓��Z�b�g
		m_attackGapPreviousInput = NONE_INPUT;
	}

	//��Ɏ����Ă��ԂȂ牽�����Ȃ�
	if (m_status == HAND)return;

	//��s���͂��������ꍇ�A���f�\�Ȃ瑦���ڍs
	if (m_tripleAttackPreviousInput && CanInterrupt())
	{
		Throw(arg_playersVecX);
		m_tripleAttackPreviousInput = false;
	}

	//��ɖ߂�����ԂɑJ��
	if (m_timer.UpdateTimer(timeScale))
	{
		m_neutralCol->SetActive(false);
		m_throwCol->SetActive(false);
		m_status = HAND;
	}

	//�����̉����x�X�V
	m_accel = KuroMath::Ease(Out, Exp, m_timer.GetTimeRate(), m_iniAccel, { 0,0,0 });
}

#include"BasicDraw.h"
void YoYo::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam, const IndividualDrawParameter& arg_toonParam)
{
	BasicDraw::Instance()->Draw(*arg_lightMgr.lock(), m_modelObj, *arg_cam.lock(), arg_toonParam);
}

#include"imguiApp.h"
void YoYo::AddImguiDebugItem()
{
	//���݂̃X�e�[�^�X
	ImGui::Text("Status : { %s }", std::string(magic_enum::enum_name(m_status)).c_str());
	ImGui::Text("Rate : { %f }", m_timer.GetTimeRate());
	ImGui::Text("Elasped : { %f }", m_timer.GetElaspedTime());
	ImGui::Separator();

	//��s����
	ImGui::Text("PreviousInput : { %s }", m_tripleAttackPreviousInput ? "true" : "false");

	ImGui::Separator();

	for (int i = 0; i < (int)STATUS::STATUS_NUM; ++i)
	{
		STATUS status = (STATUS)i;
		auto& param = m_statusParams[i];
		if (ImGui::TreeNode(std::string(magic_enum::enum_name(status)).c_str()))
		{
			ImGui::DragInt("FinishInterval", &param.m_finishInterval);
			ImGui::DragInt("InterruptInterval", &param.m_interruptInterval);
			if (param.m_finishInterval < param.m_interruptInterval)
				param.m_interruptInterval = param.m_finishInterval;

			ImGui::DragFloat("AccelX", &param.m_maxAccelVal.x);
			if (param.m_maxAccelVal.x < 0.0f)param.m_maxAccelVal.x = 0.0f;
			ImGui::DragFloat("AccelY", &param.m_maxAccelVal.y);
			if (param.m_maxAccelVal.y < 0.0f)param.m_maxAccelVal.y = 0.0f;

			ImGui::TreePop();
		}
	}
}

bool YoYo::Throw(float arg_vecX)
{
	//�C���^�[�o���i�U���̌��j
	if (IsAttackGapInterval(THROW_INPUT))return false;

	//�U������
	if (IsThrow())
	{
		//���f�\�łȂ�
		if (!CanInterrupt())
		{	
			//��s���͂Ƃ��Ď�t
			m_tripleAttackPreviousInput = true;
			return false;
		}
	}

	if (!(m_status == HAND || m_status <= THROW_1))return false;

	//�����p�R���C�_�[�A�N�e�B�u
	m_throwCol->SetActive(true);

	if (arg_vecX <= 0.0f)
	{
		//������
		m_vecTransform.SetRotate(XMMatrixIdentity());
	}
	else
	{
		//�E����
		m_vecTransform.SetRotate(Angle(0), Angle(180), Angle(0));
	}

	if (m_status == HAND)m_status = THROW_0;
	else if(m_status != THROW_2) m_status = (STATUS)(m_status + 1);

	StatusInit({ arg_vecX <= 0.0f ? -1.0f : 1.0f,1.0f,0.0f });

	return true;
}

bool YoYo::Neutral()
{
	//�C���^�[�o���i�U���̌��j
	if (IsAttackGapInterval(NEUTRAL_INPUT))return false;

	m_status = NEUTRAL;
	//N�U���R���C�_�[�A�N�e�B�u
	m_neutralCol->SetActive(true);

	//�������̃R���C�_�[���A�N�e�B�u��
	m_throwCol->SetActive(false);

	//�X�e�[�^�X�̃p�����[�^�擾
	auto& param = m_statusParams[(int)m_status];

	StatusInit({ 0.0f,0.0f,0.0f });

	return true;
}
