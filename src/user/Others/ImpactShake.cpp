#include "ImpactShake.h"
#include"KuroFunc.h"

void ImpactShake::Update(float arg_timeScale)
{
	//�U����
	if (!m_activeTimer.UpdateTimer(arg_timeScale))
	{
		//�U���X�p���v��
		if (m_spanTimer.UpdateTimer(arg_timeScale))
		{
			//���Ԃ��o�قǐU�����キ�Ȃ��Ă���
			float timeRate = m_activeTimer.GetTimeRate();
			float shakePowerScale = KuroMath::Ease(Out, Circ, timeRate, 1.0f, 0.0f);

			//�U���I�t�Z�b�g�l�����߂�
			m_offset.x = KuroFunc::GetRand(m_powerMin, m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();
			m_offset.y = KuroFunc::GetRand(m_powerMin, m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();

			m_spanTimer.Reset();
		}
	}
	else
	{
		//�U���I���
		m_offset = { 0,0,0 };
	}
}

void ImpactShake::Shake(float arg_time, float arg_span, float arg_powerMin, float arg_powerMax)
{
	//�e��U���p�����[�^�Z�b�g
	m_activeTimer.Reset(arg_time);
	m_spanTimer.Reset(arg_span);
	m_powerMin = arg_powerMin;
	m_powerMax = arg_powerMax;

	//�U���I�t�Z�b�g�����l�v�Z
	m_offset.x = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
	m_offset.y = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
}
