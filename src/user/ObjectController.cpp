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

void OC_DirectionMove::OnInit(GameObject& arg_obj)
{
	arg_obj.m_transform.SetPos(m_startPos);
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

void OC_TargetObjectEaseMove::SetParameters(Vec3<float> arg_startPos, GameObject* arg_target)
{
	m_startPos = arg_startPos;
	m_target = arg_target;
	if (m_target)m_destinationPos = m_target->m_transform.GetPos();
}

float OC_SlimeBattery::GetNearEdgePosX(float arg_posX)const
{
	using namespace ConstParameter::GameObject;

	//�ŏ��̈ʒu���E�[�ƍ��[�ǂ��炪�߂������ׂāA�߂�����X���W�ɍ��킹��
	float distRight = abs(m_spotXList[m_spotXIdx] - POS_X_ABS);
	float distLeft = abs(m_spotXList[m_spotXIdx] - -POS_X_ABS);
	//�E�̕����߂�
	if (distRight < distLeft)
	{
		return POS_X_ABS;
	}
	//���̂ق����߂�
	return POS_X_ABS;
}

void OC_SlimeBattery::OnInit(GameObject& arg_obj)
{
	using namespace ConstParameter::Environment;

	//��ђ��ˈʒu�C���f�b�N�X������
	m_spotXIdx = 0;

	//�o��X�e�[�^�X
	m_status = APPEAR;

	//�����ʒu
	Vec3<float>initPos = { 0.0f,m_appearY,FIELD_FLOOR_POS.z };

	initPos.x = GetNearEdgePosX(m_spotXList[m_spotXIdx]);

	//�����ʒu�ݒ�
	arg_obj.m_transform.SetPos(initPos);

	//�o��^�C�}�[�ݒ�
	m_timer.Reset(m_intervalTimes[STATUS::APPEAR]);

	//�ړ��ʏ�����
	m_moveY = 0.0f;

	//�o���n�_X
	m_departureX = initPos.x;

	//��ђ��˖ڕW�n�_X
	m_destinationX = m_spotXList[m_spotXIdx];
}

void OC_SlimeBattery::OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Environment;

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
		const float MY_RADIUS = 1.0f;
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
	if (m_timer.IsTimeUp() && m_status != DISAPPEAR)
	{
		//�ҋ@��
		if (m_status == STATUS::APPEAR || m_status == STATUS::JUMP_AND_SHOT)
		{
			m_status = WAIT;
		}
		//��ђ��˂�
		else if (m_status == WAIT)
		{
			//���̒n�_�����邩
			if ((m_spotXIdx + 1) < static_cast<int>(m_spotXList.size()))
			{
				//���̒n�_
				m_destinationX = m_spotXList[++m_spotXIdx];
				m_status = JUMP_AND_SHOT;
			}
			//�ޏ��
			else
			{
				//�߂��[�֌�����
				m_destinationX = GetNearEdgePosX(pos.x);
				m_status = DISAPPEAR;
			}

			//��ђ���
			m_moveY = m_jumpPower;
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

		//���Ԃ̃Z�b�g
		m_timer.Reset(m_intervalTimes[m_status]);
	}
}

std::unique_ptr<ObjectController> OC_SlimeBattery::Clone()
{
	return std::make_unique<OC_SlimeBattery>(m_intervalTimes.data(), m_jumpPower);
}

bool OC_SlimeBattery::IsLeave(GameObject& arg_obj) const
{
	//�ޏꒆ����Ȃ����false
	if (m_status != DISAPPEAR)return false;
	//�ޏ�ς�
	return m_timer.IsTimeUp();
}