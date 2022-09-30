#include "GameCamera.h"
#include"KuroFunc.h"
#include"KuroMath.h"
#include<math.h>

void GameCamera::SetPosAndTarget(Vec3<float>arg_offset)
{
	for (int i = 0; i < NUM; ++i)
	{
		m_cam[i]->SetPos(m_defaultPos[i] + arg_offset);
		m_cam[i]->SetTarget(m_targetPos[i] + arg_offset);
	}
}


GameCamera::GameCamera()
{
	m_cam[BACK] = std::make_shared<Camera>("GameCamera - Back");
	m_cam[FRONT] = std::make_shared<Camera>("GameCamera - Front");
}

void GameCamera::Init()
{
	for (int i = 0; i < NUM; ++i)
	{
		m_cam[i]->SetAngleOfView(Angle(m_defaultCapeView[i]));
	}
	SetPosAndTarget();
}

void GameCamera::Update(float arg_timeScale)
{
	//�U����
	if (!m_shake.m_activeTimer.UpdateTimer(arg_timeScale))
	{
		//�U���X�p���v��
		if (m_shake.m_spanTimer.UpdateTimer(arg_timeScale))
		{
			//���Ԃ��o�قǐU�����キ�Ȃ��Ă���
			float timeRate = m_shake.m_activeTimer.GetTimeRate();
			float shakePowerScale = KuroMath::Ease(Out, Circ, timeRate, 1.0f, 0.0f);

			//�U���I�t�Z�b�g�l�����߂�
			Vec3<float>offset = { 0,0,0 };
			offset.x = KuroFunc::GetRand(m_shake.m_powerMin, m_shake.m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();
			offset.y = KuroFunc::GetRand(m_shake.m_powerMin, m_shake.m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();

			//�U���I�t�Z�b�g�K�p
			SetPosAndTarget(offset);

			m_shake.m_spanTimer.Reset(m_shake.m_span);
		}
	}

	//�U�����I������猳�̍��W�ɖ߂�
	if (m_shake.m_activeTimer.IsTimeUpOnTrigger())
	{
		for (int i = 0; i < NUM; ++i)
		{
			m_cam[i]->SetPos(m_defaultPos[i]);
			m_cam[i]->SetTarget(m_targetPos[i]);
		}
	}
}

void GameCamera::Shake(int arg_time, int arg_span, float arg_powerMin, float arg_powerMax)
{
	//�e��U���p�����[�^�Z�b�g
	m_shake.m_activeTimer.Reset(arg_time);
	m_shake.m_span = arg_span;
	m_shake.m_spanTimer.Reset(arg_span);
	m_shake.m_powerMin = arg_powerMin;
	m_shake.m_powerMax = arg_powerMax;

	//�U���I�t�Z�b�g�����l�v�Z
	Vec3<float>offset = { 0,0,0 };
	offset.x = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
	offset.y = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();

	//�U���I�t�Z�b�g�K�p
	SetPosAndTarget(offset);
}
