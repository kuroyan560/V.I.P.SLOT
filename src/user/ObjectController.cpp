#include"ObjectController.h"
#include"GameObject.h"
#include"ObjectBreed.h"
#include"ConstParameters.h"
#include"TimeScale.h"
#include"ObjectManager.h"

std::weak_ptr<ObjectManager>ObjectController::s_objMgr;

#include"BasicDraw.h"
void ObjectController::OnDraw(GameObject& arg_obj, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	BasicDraw::Draw(*arg_lightMgr.lock(), arg_obj.m_breed.lock()->m_model, arg_obj.m_transform, *arg_cam.lock(), arg_obj.m_drawParam);
}

bool ObjectController::IsObjsHpZero(GameObject& arg_obj) const
{
	return arg_obj.m_hp <= 0;
}

bool ObjectController::IsOutOfScreen(GameObject& arg_obj)const
{
	using namespace ConstParameter::GameObject;

	auto pos = arg_obj.m_transform.GetPos();
	//フィールド外
	if (pos.x < -POS_X_ABS)return true;
	if (POS_X_ABS < pos.x)return true;
	if (pos.y < POS_Y_MIN)return true;
	if (POS_Y_MAX < pos.y)return true;
	return false;
}

float ObjectController::GetLocalTimeScale(GameObject& arg_obj)const
{
	return KuroMath::Ease(In, Sine, arg_obj.m_damageTimer.GetTimeRate(), 0.0f, 1.0f);
}

void OC_DirectionMove::OnInit(GameObject& arg_obj)
{
	arg_obj.m_transform.SetPos(m_startPos);
	m_radian = Angle::PI() / 2.0f;
}

void OC_DirectionMove::OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//タイムスケール取得
	float timeScale = arg_timeScale.GetTimeScale() * GetLocalTimeScale(arg_obj);

	//移動方向XY平面
	Vec2<float>moveDirXY = m_moveDirXY;

	//蛇行
	if (m_sinMeandeling)
	{
		m_radian += Angle::ROUND() / m_meandelingInterval * timeScale;
		moveDirXY = KuroMath::RotateVec2(moveDirXY, sin(m_radian) * m_meandelingAngle);
	}

	//座標
	auto pos = arg_obj.m_transform.GetPos();
	pos.x += moveDirXY.x * m_speed * timeScale;
	pos.y += moveDirXY.y * m_speed * timeScale;
	arg_obj.m_transform.SetPos(pos);
}

std::unique_ptr<ObjectController> OC_DirectionMove::Clone()
{
	auto clone = std::make_unique<OC_DirectionMove>();
	clone->SetParameters(m_startPos, m_moveDirXY, m_speed, m_sinMeandeling);
	return clone;
}

void OC_DestinationEaseMove::OnInit(GameObject& arg_obj)
{
	arg_obj.m_transform.SetPos(m_startPos);
	m_timer.Reset(m_interval);
	m_isGoal = false;
}

void OC_DestinationEaseMove::OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	if (m_timer.IsTimeUp())m_isGoal = true;

	m_timer.UpdateTimer(arg_timeScale.GetTimeScale());
	auto pos = KuroMath::Ease(m_easeChangeType, m_easeType, m_timer.GetTimeRate(), m_startPos, m_destinationPos);
	arg_obj.m_transform.SetPos(pos);
}

std::unique_ptr<ObjectController> OC_DestinationEaseMove::Clone()
{
	return std::make_unique<OC_DestinationEaseMove>(m_easeChangeType, m_easeType, m_interval);
}

void OC_TargetObjectEaseMove::OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	float timeScale = arg_timeScale.GetTimeScale() * GetLocalTimeScale(arg_obj);

	if (m_timer.IsTimeUp())m_isGoal = true;

	//ターゲットが生きている
	if (m_target && !m_target->IsDead())
	{
		//目的地を更新
		m_destinationPos = m_target->m_transform.GetPos();
	}
	m_timer.UpdateTimer(timeScale);
	auto pos = KuroMath::Ease(m_easeChangeType, m_easeType, m_timer.GetTimeRate(), m_startPos, m_target->m_transform.GetPos());
	arg_obj.m_transform.SetPos(pos);
}

std::unique_ptr<ObjectController> OC_TargetObjectEaseMove::Clone()
{
	return std::make_unique<OC_TargetObjectEaseMove>(m_easeChangeType, m_easeType, m_interval);
}

void OC_TargetObjectEaseMove::SetParameters(Vec3<float> arg_startPos, GameObject* arg_target)
{
	m_startPos = arg_startPos;
	m_target = arg_target;
	if (m_target)m_destinationPos = m_target->m_transform.GetPos();
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

void OC_SlimeBattery::OnInit(GameObject& arg_obj)
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
	arg_obj.m_transform.SetPos(initPos);

	//登場タイマー設定
	m_timer.Reset(m_intervalTimes[STATUS::APPEAR]);

	//移動量初期化
	m_moveY = 0.0f;

	//出発地点X
	m_departureX = initPos.x;

	//飛び跳ね目標地点X
	m_destinationX = m_spotXList[m_spotXIdx];
}

void OC_SlimeBattery::OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Environment;

	//座標取得
	auto pos = arg_obj.m_transform.GetPos();

	//タイムスケール取得
	float timeScale = arg_timeScale.GetTimeScale() * GetLocalTimeScale(arg_obj);

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
		arg_obj.m_transform.SetPos(pos);
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
			//ショット
			s_objMgr.lock()->AppearEnemyBullet(
				arg_collisionMgr,
				&arg_obj,
				pos,
				Vec2<float>(0.0f, 1.0f),
				0.2f,
				true,
				60.0f,
				Angle(30));
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