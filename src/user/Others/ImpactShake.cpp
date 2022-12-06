#include "ImpactShake.h"
#include"KuroFunc.h"

void ImpactShake::Update(float arg_timeScale)
{
	//振動中
	if (!m_activeTimer.UpdateTimer(arg_timeScale))
	{
		//振動スパン計測
		if (m_spanTimer.UpdateTimer(arg_timeScale))
		{
			//時間が経つほど振動が弱くなっていく
			float timeRate = m_activeTimer.GetTimeRate();
			float shakePowerScale = KuroMath::Ease(Out, Circ, timeRate, 1.0f, 0.0f);

			//振動オフセット値を求める
			m_offset.x = KuroFunc::GetRand(m_powerMin, m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();
			m_offset.y = KuroFunc::GetRand(m_powerMin, m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();

			m_spanTimer.Reset();
		}
	}
	else
	{
		//振動終わり
		m_offset = { 0,0,0 };
	}
}

void ImpactShake::Shake(float arg_time, float arg_span, float arg_powerMin, float arg_powerMax)
{
	//各種振動パラメータセット
	m_activeTimer.Reset(arg_time);
	m_spanTimer.Reset(arg_span);
	m_powerMin = arg_powerMin;
	m_powerMax = arg_powerMax;

	//振動オフセット初期値計算
	m_offset.x = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
	m_offset.y = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
}
