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

	//初期化位置
	Vec3<float>initPos;

	//移動方向乱数で決定
	m_xMove = KuroFunc::GetRandPlusMinus() * m_xSpeed;

	//移動スピードによってスタートXを変える
	if (m_xMove < 0.0f)
	{
		initPos.x = POS_X_ABS;
	}
	else if (0.0f < m_xMove)
	{
		initPos.x = -POS_X_ABS;
	}

	//高さランダム
	initPos.y = KuroFunc::GetRand(POS_Y_MIN + 3.0f, POS_Y_MAX);

	//フィールドのZに合わせる
	initPos.z = POS_Z;

	//初期位置設定
	arg_enemy.m_transform.SetPos(initPos);

	printf("Appear：EnemySlideMove：xMove %.f\n", m_xMove);
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
	//フィールド外
	return pos.x < -POS_X_ABS || POS_X_ABS < pos.x;
}
