#include "ClearWave.h"
#include"TimeScale.h"
#include"KuroMath.h"
#include"KuroEngine.h"
#include"Camera.h"
#include"GameCamera.h"
#include"Player.h"

void ClearWave::OnStart()
{
	m_focusTimer.Reset(30.0f);
	m_waitTimer.Reset(120.0f);
	*m_cam = *m_gameCam.lock()->GetMainCam();

	m_cam->SetPos(m_player.lock()->GetCenterPos() + Vec3<float>(0.0f, 0.0f, -10.0f));
	m_cam->SetTarget(m_player.lock()->GetCenterPos());
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

std::shared_ptr<Camera> ClearWave::GetSubCam()
{
	return m_gameCam.lock()->GetSubCam();
}

ClearWave::ClearWave() : Event(false, false)
{
	m_cam = std::make_shared<Camera>("ClearWave - Event - Camera");
}
