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

bool ObjectController::IsObjsHpZero(GameObject& arg_obj) const
{
	return arg_obj.m_hp <= 0;
}

void OC_SlideMove::OnInit(GameObject& arg_enemy)
{
	using namespace ConstParameter::GameObject;
	using namespace ConstParameter::Environment;

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
	initPos.y = KuroFunc::GetRand(FIELD_HEIGHT_MIN + 3.0f, FIELD_HEIGHT_MAX);

	//フィールドのZに合わせる
	initPos.z = FIELD_FLOOR_POS.z;

	//初期位置設定
	arg_enemy.m_transform.SetPos(initPos);

	printf("Appear：EnemySlideMove：xMove %.f\n", m_xMove);
}

void OC_SlideMove::OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)
{
	auto pos = arg_enemy.m_transform.GetPos();
	pos.x += m_xMove * arg_timeScale.GetTimeScale();
	arg_enemy.m_transform.SetPos(pos);
}

std::unique_ptr<ObjectController> OC_SlideMove::Clone()
{
	return std::make_unique<OC_SlideMove>(m_xSpeed);
}

bool OC_SlideMove::IsLeave(GameObject& arg_obj) const
{
	using namespace ConstParameter::GameObject;

	auto pos = arg_obj.m_transform.GetPos();
	//フィールド外
	return pos.x < -POS_X_ABS || POS_X_ABS < pos.x;
}

float OC_SlimeBattery::GetNearEdgePosX(float arg_posX)const
{
	using namespace ConstParameter::GameObject;

	//最初の位置が右端と左端どちらが近いか調べて、近い方のX座標に合わせる
	float distRight = abs(m_spotXList[m_spotXIdx] - POS_X_ABS);
	float distLeft = abs(m_spotXList[m_spotXIdx] - -POS_X_ABS);
	//右の方が近い
	if (distRight < distLeft)
	{
		return POS_X_ABS;
	}
	//左のほうが近い
	return POS_X_ABS;
}

void OC_SlimeBattery::OnInit(GameObject& arg_enemy)
{
	using namespace ConstParameter::Environment;

	//飛び跳ね位置インデックス初期化
	m_spotXIdx = 0;

	//登場ステータス
	m_status = APPEAR;

	//初期位置
	Vec3<float>initPos = { 0.0f,m_appearY,FIELD_FLOOR_POS.z };

	initPos.x = GetNearEdgePosX(m_spotXList[m_spotXIdx]);

	//初期位置設定
	arg_enemy.m_transform.SetPos(initPos);

	//登場タイマー設定
	m_timer.Reset(m_intervalTimes[STATUS::APPEAR]);

	//移動量初期化
	m_moveY = 0.0f;

	//出発地点X
	m_departureX = initPos.x;

	//飛び跳ね目標地点X
	m_destinationX = m_spotXList[m_spotXIdx];
}

void OC_SlimeBattery::OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Environment;

	//座標取得
	auto pos = arg_enemy.m_transform.GetPos();

	//タイムスケール取得
	float timeScale = arg_timeScale.GetTimeScale();

	//時間経過
	m_timer.UpdateTimer(timeScale);

	if (m_status != WAIT)
	{
		//加速度
		float accelY = 0.0f;
		float d;
		float t = m_timer.GetLeftTime();

		// d = (目標位置) - (現在の位置)
		const float MY_RADIUS = 1.0f;
		d = (FIELD_FLOOR_TOP_SURFACE_HEIGHT + MY_RADIUS) - pos.y;
		if (t != 0.0f)accelY = 2 * (d - m_moveY * t) / (t * t);

		//加速度加算
		m_moveY += accelY * timeScale;

		//座標更新
		pos.x = KuroMath::Ease(In, Liner,
			m_timer.GetTimeRate(), m_departureX, m_destinationX);
		pos.y += m_moveY * timeScale;
		arg_enemy.m_transform.SetPos(pos);
	}

	//時間経過
	if (m_timer.IsTimeUp() && m_status != DISAPPEAR)
	{
		//待機へ
		if (m_status == STATUS::APPEAR || m_status == STATUS::JUMP_AND_SHOT)
		{
			m_status = WAIT;
		}
		//飛び跳ねへ
		else if (m_status == WAIT)
		{
			//次の地点があるか
			if ((m_spotXIdx + 1) < static_cast<int>(m_spotXList.size()))
			{
				//次の地点
				m_destinationX = m_spotXList[++m_spotXIdx];
				m_status = JUMP_AND_SHOT;
			}
			//退場へ
			else
			{
				//近い端へ向かう
				m_destinationX = GetNearEdgePosX(pos.x);
				m_status = DISAPPEAR;
			}

			//飛び跳ね
			m_moveY = m_jumpPower;
			//出発地点の記録
			m_departureX = pos.x;
		}

		//時間のセット
		m_timer.Reset(m_intervalTimes[m_status]);
	}
}

std::unique_ptr<ObjectController> OC_SlimeBattery::Clone()
{
	return std::make_unique<OC_SlimeBattery>(m_intervalTimes.data(), m_jumpPower);
}

bool OC_SlimeBattery::IsLeave(GameObject& arg_obj) const
{
	//退場中じゃなければfalse
	if (m_status != DISAPPEAR)return false;
	//退場済か
	return m_timer.IsTimeUp();
}

