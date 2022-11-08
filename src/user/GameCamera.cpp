#include "GameCamera.h"
#include"KuroFunc.h"
#include"KuroMath.h"
#include<math.h>

void GameCamera::SetPosAndTarget(Vec3<float>arg_absOffset, Vec3<float>arg_lerpOffset)
{
	//�����͂��܂茩���Ȃ��悤�ɍ����␳
	Vec3<float>heightOffset = { 0.0f,(1.0f - std::min(arg_lerpOffset.y / 10.0f, 1.0f)) * 8.0f,0.0f };

	//�O�i�J����
	Vec3<float>pos = m_cam[FRONT]->GetPos();
	pos = KuroMath::Lerp(pos, m_defaultPos[FRONT] + heightOffset + arg_lerpOffset, 0.08f);
	m_cam[FRONT]->SetPos(pos + arg_absOffset);

	Vec3<float>target = m_cam[FRONT]->GetTarget();
	target = KuroMath::Lerp(target, m_targetPos[FRONT] + heightOffset + arg_lerpOffset, 0.08f);
	m_cam[FRONT]->SetTarget(target + arg_absOffset);

	//�w�i�J����
	m_cam[BACK]->SetPos(m_defaultPos[BACK] + arg_absOffset);
	m_cam[BACK]->SetTarget(m_targetPos[BACK] + arg_absOffset);
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
	SetPosAndTarget({ 0,0,0 }, { 0,0,0 });

	m_defaultPos[FRONT] = Vec3<float>(0.0f, 3.4f, -59.0f);
	m_targetPos[FRONT] = Vec3<float>(0.0f, m_defaultPos[FRONT].y + 2.0f, 0.0f);
}

void GameCamera::Update(float arg_timeScale, Vec3<float>arg_playersDisplacement)
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
			m_shake.m_offset.x = KuroFunc::GetRand(m_shake.m_powerMin, m_shake.m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();
			m_shake.m_offset.y = KuroFunc::GetRand(m_shake.m_powerMin, m_shake.m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();

			m_shake.m_spanTimer.Reset(m_shake.m_span);
		}
	}
	else
	{
		//�U���I���
		m_shake.m_offset = { 0,0,0 };
	}

	SetPosAndTarget(m_shake.m_offset, arg_playersDisplacement);
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
	m_shake.m_offset.x = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
	m_shake.m_offset.y = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
}
