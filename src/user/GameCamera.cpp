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
	//振動中
	if (!m_shake.m_activeTimer.UpdateTimer(arg_timeScale))
	{
		//振動スパン計測
		if (m_shake.m_spanTimer.UpdateTimer(arg_timeScale))
		{
			//時間が経つほど振動が弱くなっていく
			float timeRate = m_shake.m_activeTimer.GetTimeRate();
			float shakePowerScale = KuroMath::Ease(Out, Circ, timeRate, 1.0f, 0.0f);

			//振動オフセット値を求める
			Vec3<float>offset = { 0,0,0 };
			offset.x = KuroFunc::GetRand(m_shake.m_powerMin, m_shake.m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();
			offset.y = KuroFunc::GetRand(m_shake.m_powerMin, m_shake.m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();

			//振動オフセット適用
			SetPosAndTarget(offset);

			m_shake.m_spanTimer.Reset(m_shake.m_span);
		}
	}

	//振動が終わったら元の座標に戻す
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
	//各種振動パラメータセット
	m_shake.m_activeTimer.Reset(arg_time);
	m_shake.m_span = arg_span;
	m_shake.m_spanTimer.Reset(arg_span);
	m_shake.m_powerMin = arg_powerMin;
	m_shake.m_powerMax = arg_powerMax;

	//振動オフセット初期値計算
	Vec3<float>offset = { 0,0,0 };
	offset.x = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
	offset.y = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();

	//振動オフセット適用
	SetPosAndTarget(offset);
}
