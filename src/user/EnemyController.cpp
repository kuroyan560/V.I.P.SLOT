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
	const float MIN_X = -10.0f;
	const float MAX_X = 10.0f;

	//初期化位置
	Vec3<float>initPos;

	//移動方向乱数で決定
	m_xMove = KuroFunc::GetRandPlusMinus() * m_xSpeed;

	//移動スピードによってスタートXを変える
	if (m_xMove < 0.0f)
	{
		initPos.x = MAX_X;
	}
	else if (0.0f < m_xMove)
	{
		initPos.x = MIN_X;
	}

	//高さランダム
	const float MIN_Y = 0.0f;
	const float MAX_Y = 10.0f;
	initPos.y = KuroFunc::GetRand(MIN_Y, MAX_Y);

	//フィールドのZに合わせる
	initPos.z = ConstParameter::Environment::FIELD_DEPTH;

	//初期位置設定
	arg_enemy.m_transform.SetPos(initPos);

	printf("Appear：EnemySlideMove：xMove %.f\n", m_xMove);
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
	const float MIN_X = -10.0f;
	const float MAX_X = 10.0f;
	auto pos = arg_enemy.m_transform.GetPos();

	//フィールド外
	return pos.x < MIN_X || MAX_X < pos.x;
}
