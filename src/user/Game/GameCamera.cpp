#include "GameCamera.h"
#include"KuroFunc.h"
#include"KuroMath.h"
#include<math.h>

void GameCamera::SetPosAndTarget(Vec3<float>arg_lerpOffset, float arg_timeScale)
{
	Vec3<float>offset = arg_lerpOffset;

	//�����͂��܂茩���Ȃ��悤�ɍ����␳
	const float CORRECT_HEIGHT = 4.5f;
	offset.y += (1.0f - std::min(arg_lerpOffset.y / 10.0f, 1.0f)) * CORRECT_HEIGHT;

	//�O�i�J����
	m_posLerpOffset = KuroMath::Lerp(m_posLerpOffset, offset, 0.05f * arg_timeScale);
	auto newPos = KuroMath::Lerp(m_cam[MAIN]->GetPos() - m_shake.m_oldOffset, m_defaultPos[MAIN] + m_posLerpOffset, 0.1f * arg_timeScale);
	m_cam[MAIN]->SetPos(newPos + m_shake.m_offset);

	m_targetLerpOffset = KuroMath::Lerp(m_targetLerpOffset, offset, 0.08f * arg_timeScale);
	auto newTarget = KuroMath::Lerp(m_cam[MAIN]->GetTarget() - m_shake.m_oldOffset, m_targetPos[MAIN] + m_targetLerpOffset, 0.1f * arg_timeScale);
	m_cam[MAIN]->SetTarget(newTarget + m_shake.m_offset);

	//�w�i�J����
	m_cam[SUB]->SetPos(m_defaultPos[SUB] + m_shake.m_offset);
	m_cam[SUB]->SetTarget(m_targetPos[SUB] + m_shake.m_offset);
}


GameCamera::GameCamera()
{
	m_cam[SUB] = std::make_shared<Camera>("GameCamera - Back");
	m_cam[MAIN] = std::make_shared<Camera>("GameCamera - Front");
}

void GameCamera::Init()
{
	for (int i = 0; i < NUM; ++i)
	{
		m_cam[i]->SetAngleOfView(Angle(m_defaultCapeView[i]));
	}

	m_defaultPos[MAIN] = Vec3<float>(0.0f, 3.4f, -65.0f);
	m_targetPos[MAIN] = Vec3<float>(0.0f, m_defaultPos[MAIN].y + 2.0f, 0.0f);
	m_posLerpOffset = { 0,0,0 };
	m_targetLerpOffset = { 0,0,0 };
	m_shake.Init();
	SetPosAndTarget({ 0,0,0 }, 1.0f);
}

void GameCamera::Update(float arg_timeScale, Vec3<float>arg_playersDisplacement)
{
	//1�t���[���O�̐U���I�t�Z�b�g���L�^
	m_shake.m_offset = m_shake.m_oldOffset;
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

	SetPosAndTarget( arg_playersDisplacement,1.0f);
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
