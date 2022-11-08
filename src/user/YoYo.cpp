#include "YoYo.h"
#include"Collision.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"TimeScale.h"
#include<magic_enum.h>
#include"Importer.h"
#include"Object.h"

YoYo::YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform)
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

	//�R���C�_�[�o�^
	arg_collisionMgr.lock()->Register(m_neutralCol);
	arg_collisionMgr.lock()->Register(m_throwCol);
}


void YoYo::Awake(float arg_neutralColSphereRadius, float arg_throwColSphereRadius)
{
	static const std::string TAG = "Player_Attack";
	std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;

	//N�U���p�R���C�_�[����
	m_neutralColSphere = std::make_shared<CollisionSphere>(arg_neutralColSphereRadius, Vec3<float>(0, 0, 0));
	colPrimitiveArray = { m_neutralColSphere };
	m_neutralCol->Generate("YoYo - Neutral", TAG, colPrimitiveArray);

	//�����p�R���C�_�[����
	m_throwColSphere = std::make_shared<CollisionSphere>(arg_throwColSphereRadius, Vec3<float>(0, 0, 0));
	colPrimitiveArray = { m_throwColSphere };
	m_throwCol->Generate("YoYo - Throw", TAG, colPrimitiveArray);
}

void YoYo::Init()
{
	//�A�j���[�V����������
	m_modelObj->m_animator->Reset();

	//�R���C�_�[�I�t��
	m_throwCol->SetActive(false);

	//��Ɏ����Ă���
	m_status = HAND;
}

void YoYo::Update(const TimeScale& arg_timeScale)
{
	//�A�j���[�V�����X�V
	m_modelObj->m_animator->Update();

	//��Ɏ����Ă��ԂȂ牽�����Ȃ�
	if (m_status == HAND)return;

	//��ɖ߂�����ԂɑJ��
	if (m_timer.UpdateTimer(arg_timeScale.GetTimeScale()))
	{
		m_throwCol->SetActive(false);
		m_status = HAND;
	}
}

#include"DrawFunc3D.h"
void YoYo::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock());
}

#include"imguiApp.h"
void YoYo::AddImguiDebugItem()
{
	ImGui::Text("Status : { %s }", std::string(magic_enum::enum_name(m_status)).c_str());
	ImGui::Text("TimeRate : { %f }", m_timer.GetTimeRate());
}

void YoYo::Throw(Vec2<float>arg_vec)
{
	//if (arg_vec.IsZero())
	//{
	//	m_status = NEUTRAL;
	//}
	//else
	{
		static const Vec2<float>LEFT_VEC = { -1.0f,0.0f };
		static const Vec2<float>RIGHT_VEC = { 1.0f,0.0f };

		//�X�e�B�b�N��������U�����E�����I��
		const auto lefAngle = KuroMath::GetAngleAbs(arg_vec, LEFT_VEC);
		const auto rightAngle = KuroMath::GetAngleAbs(arg_vec, RIGHT_VEC);

		if (lefAngle < rightAngle)
		{
			//������
			m_vecTransform.SetRotate(XMMatrixIdentity());
		}
		else
		{
			//�E����
			m_vecTransform.SetRotate(Angle(0), Angle(180), Angle(0));
		}

		if (m_status == HAND)
		{
			m_status = THROW_0;
			m_throwCol->SetActive(true);
			m_modelObj->m_animator->Play("Attack_0", false, false);
		}
		else if (m_status == THROW_0)
		{
			m_status = THROW_1;
			m_modelObj->m_animator->Play("Attack_1", false, false);
		}
		else if (m_status == THROW_1)
		{
			m_status = THROW_2;
			m_modelObj->m_animator->Play("Attack_2", false, false);
		}

		//�f�o�b�O�p
		else if (m_status == THROW_2)
		{
			m_status = THROW_0;
			m_modelObj->m_animator->Play("Attack_0", false, false);
		}
	}

	m_timer.Reset(m_finishInterval[m_status]);
}
