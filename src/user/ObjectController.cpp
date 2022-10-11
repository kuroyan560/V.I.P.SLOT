#include"ObjectController.h"
#include"GameObject.h"
#include"DrawFunc3D.h"
#include"ObjectBreed.h"
#include"ConstParameters.h"
#include"TimeScale.h"

void ObjectController::OnDraw(GameObject& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	DrawFunc3D::DrawNonShadingModel(
		arg_enemy.m_breed.lock()->m_model,
		arg_enemy.m_transform,
		*arg_cam.lock(),
		1.0f,
		nullptr,
		AlphaBlendMode_None);
}

void ObjectSlideMove::OnInit(GameObject& arg_enemy)
{
	using namespace ConstParameter::GameObject;

	//�������ʒu
	Vec3<float>initPos;

	//�ړ����������Ō���
	m_xMove = KuroFunc::GetRandPlusMinus() * m_xSpeed;

	//�ړ��X�s�[�h�ɂ���ăX�^�[�gX��ς���
	if (m_xMove < 0.0f)
	{
		initPos.x = POS_X_ABS;
	}
	else if (0.0f < m_xMove)
	{
		initPos.x = -POS_X_ABS;
	}

	//���������_��
	initPos.y = KuroFunc::GetRand(POS_Y_MIN + 3.0f, POS_Y_MAX);

	//�t�B�[���h��Z�ɍ��킹��
	initPos.z = POS_Z;

	//�����ʒu�ݒ�
	arg_enemy.m_transform.SetPos(initPos);

	printf("Appear�FEnemySlideMove�FxMove %.f\n", m_xMove);
}

void ObjectSlideMove::OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)
{
	auto pos = arg_enemy.m_transform.GetPos();
	pos.x += m_xMove * arg_timeScale.GetTimeScale();
	arg_enemy.m_transform.SetPos(pos);
}

std::unique_ptr<ObjectController> ObjectSlideMove::Clone()
{
	return std::make_unique<ObjectSlideMove>(m_xSpeed);
}

bool ObjectSlideMove::IsDead(GameObject& arg_enemy)
{
	using namespace ConstParameter::GameObject;

	auto pos = arg_enemy.m_transform.GetPos();
	//�t�B�[���h�O
	return pos.x < -POS_X_ABS || POS_X_ABS < pos.x;
}
