#include "EnemyController.h"
#include"Enemy.h"
#include"DrawFunc3D.h"
#include"EnemyBreed.h"
#include"ConstParameters.h"

void EnemyController::OnDraw(Enemy& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	DrawFunc3D::DrawNonShadingModel(
		arg_enemy.m_breed.lock()->m_model,
		arg_enemy.m_transform,
		*arg_cam.lock(),
		1.0f,
		nullptr,
		AlphaBlendMode_None);
}

void EnemySlideMove::OnInit(Enemy& arg_enemy)
{
	const float ABS_X = 43.0f;

	//�������ʒu
	Vec3<float>initPos;

	//�ړ����������Ō���
	m_xMove = KuroFunc::GetRandPlusMinus() * m_xSpeed;

	//�ړ��X�s�[�h�ɂ���ăX�^�[�gX��ς���
	if (m_xMove < 0.0f)
	{
		initPos.x = ABS_X;
	}
	else if (0.0f < m_xMove)
	{
		initPos.x = -ABS_X;
	}

	//���������_��
	const float MIN_Y = 1.0f;
	const float MAX_Y = 32.5f;
	initPos.y = KuroFunc::GetRand(MIN_Y, MAX_Y);
	initPos.y = MIN_Y;

	//�t�B�[���h��Z�ɍ��킹��
	initPos.z = ConstParameter::Environment::FIELD_DEPTH - 5.2f;

	//�����ʒu�ݒ�
	arg_enemy.m_transform.SetPos(initPos);

	printf("Appear�FEnemySlideMove�FxMove %.f\n", m_xMove);
}

void EnemySlideMove::OnUpdate(Enemy& arg_enemy)
{
	auto pos = arg_enemy.m_transform.GetPos();
	pos.x += m_xMove;
	arg_enemy.m_transform.SetPos(pos);
}

std::unique_ptr<EnemyController> EnemySlideMove::Clone()
{
	return std::make_unique<EnemySlideMove>(m_xSpeed);
}

bool EnemySlideMove::IsDead(Enemy& arg_enemy)
{
	const float ABS_X = 43.0f;
	auto pos = arg_enemy.m_transform.GetPos();

	//�t�B�[���h�O
	return pos.x < -ABS_X || ABS_X < pos.x;
}
