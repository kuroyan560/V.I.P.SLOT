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
	//�t�B�[���h�O
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
	//�����o��
	m_life.UpdateTimer(arg_timeScale.GetTimeScale());

	//�d�͉��Z
	m_vel.y -= m_gravity;

	//�ړ��ʉ��Z
	auto pos = arg_obj.m_transform.GetPos();
	pos += m_vel;
	arg_obj.m_transform.SetPos(pos);

	//��������
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
	//�^�C���X�P�[���擾
	float timeScale = arg_timeScale.GetTimeScale() * GetLocalTimeScale(arg_obj);

	//�ړ�����XY����
	Vec2<float>moveDirXY = m_moveDirXY;

	//�֍s
	if (m_sinMeandeling)
	{
		m_radian += Angle::ROUND() / m_meandelingInterval * timeScale;
		moveDirXY = KuroMath::RotateVec2(moveDirXY, sin(m_radian) * m_meandelingAngle);
	}

	//���W
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

	//�^�[�Q�b�g�������Ă���
	if (m_target && !m_target->IsDead())
	{
		//�ړI�n���X�V
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

	//�o��X�e�[�^�X
	m_status = JUMP_AND_SHOT;

	//�����ʒu�ݒ�
	arg_obj.m_transform.SetPos(arg_initPos);

	//�o��^�C�}�[�ݒ�
	m_timer.Reset(JUMP_INTERVAL);

	//�ړ��ʏ�����
	m_moveY = 0.0f;

	//�o���n�_X
	m_departureX = arg_initPos.x;

	//��ђ��˖ڕW�n�_X
	float nextX = 0.0f;
	if (!OnDecideNextDetinationX(arg_obj, nextX))assert(0);
	m_destinationX = nextX;

	//�ޏ�t���O���Z�b�g
	m_isLeave = false;
}

void OC_SlimeBattery::OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Environment;
	using namespace ConstParameter::Enemy::SlimeBattery;

	//���W�擾
	auto pos = arg_obj.m_transform.GetPos();

	//�^�C���X�P�[���擾
	float timeScale = arg_timeScale.GetTimeScale() * GetLocalTimeScale(arg_obj);

	//���Ԍo��
	m_timer.UpdateTimer(timeScale);

	if (m_status != WAIT)
	{
		//�����x
		float accelY = 0.0f;
		float d;
		float t = m_timer.GetLeftTime();

		// d = (�ڕW�ʒu) - (���݂̈ʒu)
		const float MY_RADIUS = 1.55f;
		d = (FIELD_FLOOR_TOP_SURFACE_HEIGHT + MY_RADIUS) - pos.y;
		if (t != 0.0f)accelY = 2 * (d - m_moveY * t) / (t * t);

		//�����x���Z
		m_moveY += accelY * timeScale;

		//���W�X�V
		pos.x = KuroMath::Ease(In, Liner,
			m_timer.GetTimeRate(), m_departureX, m_destinationX);
		pos.y += m_moveY * timeScale;
		arg_obj.m_transform.SetPos(pos);
	}

	//���Ԍo��
	if (m_timer.IsTimeUp())
	{
		//�ҋ@��
		if (m_status == STATUS::JUMP_AND_SHOT)
		{
			//�X�e�[�^�X�ω�
			m_status = WAIT;
			//���Ԃ̃Z�b�g
			m_timer.Reset(WAIT_INTERVAL);
		}
		//��ђ��˂�
		else if (m_status == WAIT)
		{
			//�W�����v���̏���
			OnSlimeJump();

			//���Ԃ̃Z�b�g
			m_timer.Reset(JUMP_INTERVAL);

			//���̒n�_�����邩
			float nextX = 0.0f;	//���̒n�_X���̊i�[��ϐ�
			if (OnDecideNextDetinationX(arg_obj,nextX))
			{
				//���̒n�_
				m_destinationX = nextX;
				//�X�e�[�^�X�ω�
				m_status = JUMP_AND_SHOT;
				//��ђ���
				m_moveY = JUMP_POWER;
				//�o���n�_�̋L�^
				m_departureX = pos.x;
				//�V���b�g
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
			//�ޏ�t���OON
			else m_isLeave = true;
		}


	}
}


