#include "GameCamera.h"
#include"KuroFunc.h"
#include"KuroMath.h"
#include<math.h>

void GameCamera::SetPosAndTarget(Vec3<float>arg_lerpOffset, float arg_timeScale)
{
	Vec3<float>offset = arg_lerpOffset;

	//床下はあまり見えないように高さ補正
	const float CORRECT_HEIGHT = 4.5f;
	offset.y += (1.0f - std::min(arg_lerpOffset.y / 10.0f, 1.0f)) * CORRECT_HEIGHT;

	//前景カメラ
	m_posLerpOffset = KuroMath::Lerp(m_posLerpOffset, offset, 0.05f * arg_timeScale);
	auto newPos = KuroMath::Lerp(m_cam[MAIN]->GetPos() - m_shake.m_oldOffset, m_defaultPos[MAIN] + m_posLerpOffset, 0.1f * arg_timeScale);
	m_cam[MAIN]->SetPos(newPos + m_shake.m_offset);

	m_targetLerpOffset = KuroMath::Lerp(m_targetLerpOffset, offset, 0.08f * arg_timeScale);
	auto newTarget = KuroMath::Lerp(m_cam[MAIN]->GetTarget() - m_shake.m_oldOffset, m_targetPos[MAIN] + m_targetLerpOffset, 0.1f * arg_timeScale);
	m_cam[MAIN]->SetTarget(newTarget + m_shake.m_offset);

	//背景カメラ
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
	//1フレーム前の振動オフセットを記録
	m_shake.m_offset = m_shake.m_oldOffset;
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
			m_shake.m_offset.x = KuroFunc::GetRand(m_shake.m_powerMin, m_shake.m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();
			m_shake.m_offset.y = KuroFunc::GetRand(m_shake.m_powerMin, m_shake.m_powerMax)
				* shakePowerScale * KuroFunc::GetRandPlusMinus();

			m_shake.m_spanTimer.Reset(m_shake.m_span);
		}
	}
	else
	{
		//振動終わり
		m_shake.m_offset = { 0,0,0 };
	}

	SetPosAndTarget( arg_playersDisplacement,1.0f);
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
	m_shake.m_offset.x = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
	m_shake.m_offset.y = KuroFunc::GetRand(arg_powerMin, arg_powerMax) * KuroFunc::GetRandPlusMinus();
}
