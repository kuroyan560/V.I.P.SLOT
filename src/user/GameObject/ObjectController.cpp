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
	BasicDraw::Instance()->Draw(*arg_lightMgr.lock(), arg_obj.m_breed.lock()->m_model, arg_obj.m_transform, *arg_cam.lock(), arg_obj.m_drawParam);
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

void OC_EmitThrowOnFloor::OnInit(GameObject& arg_obj, Vec3<float> arg_initPos)
{
	arg_obj.m_transform.SetPos(arg_initPos);
	m_life.Reset();
	m_fall = false;
}

void OC_EmitThrowOnFloor::OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	//寿命経過
	m_life.UpdateTimer(arg_timeScale.GetTimeScale());

	//重力加算
	m_vel.y -= m_gravity;

	//移動量加算
	auto pos = arg_obj.m_transform.GetPos();
	pos += m_vel;
	arg_obj.m_transform.SetPos(pos);

	//落下判定
	if (pos.y < ConstParameter::Environment::FALL_LIMIT_HEIGHT)
	{
		m_fall = true;
	}
}

std::unique_ptr<ObjectController> OC_EmitThrowOnFloor::Clone()
{
	auto clone = std::make_unique<OC_EmitThrowOnFloor>(m_gravity, m_life.GetIntervalTime());
	return clone;
}

void OC_EmitThrowOnFloor::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider>arg_otherCollider)
{
}

void OC_DirectionMove::OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)
{
	arg_obj.m_transform.SetPos(arg_initPos);
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
	clone->SetParameters(m_moveDirXY, m_speed, m_sinMeandeling);
	return clone;
}

void OC_DestinationEaseMove::OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)
{
	arg_obj.m_transform.SetPos(arg_initPos);
	m_timer.Reset(m_interval);
	m_isGoal = false;
	m_startPos = arg_initPos;
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

void OC_TargetObjectEaseMove::SetParameters(GameObject* arg_target)
{
	m_target = arg_target;
	if (m_target)m_destinationPos = m_target->m_transform.GetPos();
}

void OC_SlimeBattery::OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)
{
	using namespace ConstParameter::Enemy::SlimeBattery;

	//登場ステータス
	m_status = JUMP_AND_SHOT;

	//初期位置設定
	arg_obj.m_transform.SetPos(arg_initPos);

	//登場タイマー設定
	m_timer.Reset(JUMP_INTERVAL);

	//移動量初期化
	m_moveY = 0.0f;

	//出発地点X
	m_departureX = arg_initPos.x;

	//飛び跳ね目標地点X
	float nextX = 0.0f;
	if (!OnDecideNextDetinationX(arg_obj, nextX))assert(0);
	m_destinationX = nextX;

	//退場フラグリセット
	m_isLeave = false;
}

void OC_SlimeBattery::OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Environment;
	using namespace ConstParameter::Enemy::SlimeBattery;

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
		const float MY_RADIUS = 1.55f;
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
	if (m_timer.IsTimeUp())
	{
		//待機へ
		if (m_status == STATUS::JUMP_AND_SHOT)
		{
			//ステータス変化
			m_status = WAIT;
			//時間のセット
			m_timer.Reset(WAIT_INTERVAL);
		}
		//飛び跳ねへ
		else if (m_status == WAIT)
		{
			//ジャンプ時の処理
			OnSlimeJump();

			//時間のセット
			m_timer.Reset(JUMP_INTERVAL);

			//次の地点があるか
			float nextX = 0.0f;	//次の地点X情報の格納先変数
			if (OnDecideNextDetinationX(arg_obj,nextX))
			{
				//次の地点
				m_destinationX = nextX;
				//ステータス変化
				m_status = JUMP_AND_SHOT;
				//飛び跳ね
				m_moveY = JUMP_POWER;
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
			//退場フラグON
			else m_isLeave = true;
		}


	}
}


