#include "ClearWave.h"
#include"TimeScale.h"
#include"KuroMath.h"
#include"KuroEngine.h"

void ClearWave::OnStart()
{
	m_focusTimer.Reset(30.0f);
	m_waitTimer.Reset(120.0f);
}

void ClearWave::OnUpdate(TimeScale& arg_timeScale)
{
	float timeScale = KuroMath::Lerp(1.0f, 0.5f, m_focusTimer.GetTimeRate());
	arg_timeScale.Set(timeScale);

	if (m_focusTimer.UpdateTimer())
	{
		m_waitTimer.UpdateTimer();
	}
}
void ClearWave::OnFinish()
{
	KuroEngine::Instance()->ChangeScene("OutGame");
}
