#include "YoYo.h"
#include"Collision.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"TimeScale.h"
#include<magic_enum.h>
#include"Importer.h"

YoYo::YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform)
{
	m_model = Importer::Instance()->LoadModel("resource/user/model/", "yoyo.glb");

	m_vecTransform.SetParent(arg_playerTransform);
	m_transform.SetParent(&m_vecTransform);

	m_throwCol = std::make_shared<Collider>();
	m_throwCol->SetParentTransform(&m_transform);
	arg_collisionMgr.lock()->Register(m_throwCol);
}


void YoYo::Awake(float arg_length, float arg_colSphereRadius)
{
	//�K�v�ȓ����蔻��p���̐�
	int colSphereNum = static_cast<int>(ceil(arg_length / arg_colSphereRadius));

	//�����蔻��p������
	m_capsuleSphereArray.resize(colSphereNum);

	//�I�t�Z�b�g
	float offsetX = arg_length / static_cast<float>(colSphereNum);

	//�f�t�H���g�{X�����A�n�_�O
	for (int i = 0; i < colSphereNum; ++i)
	{
		m_capsuleSphereArray[i] = std::make_shared<CollisionSphere>(arg_colSphereRadius, Vec3<float>(offsetX * i, 0.0f, 0.0f));
	}

	std::vector<std::shared_ptr<CollisionPrimitive>>capsulePrimitiveArray;
	for (auto& sp : m_capsuleSphereArray)capsulePrimitiveArray.emplace_back(sp);
	m_throwCol->Generate("YoYo", "Player_Attack", capsulePrimitiveArray);
}

void YoYo::Init()
{
	//�R���C�_�[�I�t��
	m_throwCol->SetActive(false);

	//��Ɏ����Ă���
	m_status = HAND;
}

void YoYo::Update(const TimeScale& arg_timeScale)
{
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
	DrawFunc3D::DrawNonShadingModel(m_model, m_transform, *arg_cam.lock());
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
		//����p
		static std::array<Angle, THROW_VEC_NUM>throwAngleJudge =
		{
			Angle(180),Angle(135),Angle(45),Angle(0)
		};

		THROW_VEC throwVec = LEFT;
		arg_vec.y *= -1.0f;
		auto angle = KuroMath::GetAngle(arg_vec).GetNormalize();
		float angleAbsMin = FLT_MAX;

		//���͕����ƈ�ԋ߂��p�x�𔻒�
		for (int i = 0; i < THROW_VEC_NUM; ++i)
		{
			float angleAbs = abs((angle + Angle::ConvertToRadian(270))
				- (throwAngleJudge[i].m_radian + Angle::ConvertToRadian(270)));
			if (angleAbs < angleAbsMin)
			{
				angleAbsMin = angleAbs;
				throwVec = (THROW_VEC)i;
			}
		}

		//�R���C�_�[�̌���������
		if (throwVec == RIGHT)
		{
			//�E�����͏���
			m_vecTransform.SetRotate(XMMatrixIdentity());
		}
		else if (throwVec == RIGHT_UP)
		{
			m_vecTransform.SetRotate(Angle(0), Angle(0), Angle(45));
		}
		else if (throwVec == LEFT)
		{
			m_vecTransform.SetRotate(Angle(0), Angle(180), Angle(0));
		}
		else if (throwVec == LEFT_UP)
		{
			m_vecTransform.SetRotate(Angle(0), Angle(180), Angle(45));
		}

		if (m_status == HAND)
		{
			m_status = THROW_0;
			m_throwCol->SetActive(true);
		}
		else if (m_status == THROW_0)m_status = THROW_1;
		else if (m_status == THROW_1)m_status = THROW_2;

		//�f�o�b�O�p
		else if (m_status == THROW_2)m_status = THROW_0;
	}

	m_timer.Reset(m_finishInterval[m_status]);
}
