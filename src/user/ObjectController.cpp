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
	initPos.y = KuroFunc::GetRand(FIELD_HEIGHT_MIN + 3.0f, FIELD_HEIGHT_MAX);

	//�t�B�[���h��Z�ɍ��킹��
	initPos.z = FIELD_FLOOR_POS.z;

	//�����ʒu�ݒ�
	arg_enemy.m_transform.SetPos(initPos);

	printf("Appear�FEnemySlideMove�FxMove %.f\n", m_xMove);
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
	//�t�B�[���h�O
	return pos.x < -POS_X_ABS || POS_X_ABS < pos.x;
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

void OC_SlimeBattery::OnInit(GameObject& arg_enemy)
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
	arg_enemy.m_transform.SetPos(initPos);

	//�o��^�C�}�[�ݒ�
	m_timer.Reset(m_intervalTimes[STATUS::APPEAR]);

	//�ړ��ʏ�����
	m_moveY = 0.0f;

	//�o���n�_X
	m_departureX = initPos.x;

	//��ђ��˖ڕW�n�_X
	m_destinationX = m_spotXList[m_spotXIdx];
}

void OC_SlimeBattery::OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Environment;

	//���W�擾
	auto pos = arg_enemy.m_transform.GetPos();

	//�^�C���X�P�[���擾
	float timeScale = arg_timeScale.GetTimeScale();

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
		arg_enemy.m_transform.SetPos(pos);
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

