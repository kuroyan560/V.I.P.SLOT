#include "ClearWave.h"
#include"TimeScale.h"
#include"KuroMath.h"
#include"KuroEngine.h"
#include"Camera.h"
#include"GameCamera.h"
#include"Player.h"

void ClearWave::OnStart()
{
	m_status = WAIT;

	m_timer.Reset(120.0f);
	*m_cam = *m_referGameCam.lock()->GetMainCam();

	m_camWorkIdx = 0;

	m_referTimeScale->Set(0.0f);
}

void ClearWave::UpdateCameraWork()
{
	m_timer.Reset(60.0f);
	m_cam->SetPos(m_referPlayer.lock()->GetCenterPos() + m_camWorkPosOffset[m_camWorkIdx++]);
}

void ClearWave::OnUpdate()
{
	if (m_debug && !m_preview)return;

	m_timer.UpdateTimer();

	if (m_status == WAIT)
	{
		if (m_timer.IsTimeUp())
		{
			m_status = CAM_WORK;
		}
	}
	else if (m_status == CAM_WORK)
	{
		if (m_timer.IsTimeUp())
		{
			if (static_cast<int>(m_camWorkPosOffset.size()) <= m_camWorkIdx)
			{
				m_status = SLOW;
				m_timer.Reset(120);
			}
			else
			{
				UpdateCameraWork();
			}
		}
	}
	else if (m_status == SLOW)
	{
		float timeScale = KuroMath::Lerp(0.0f, 0.5f, m_timer.GetTimeRate());
		m_referTimeScale->Set(timeScale);
	}

	if (m_debug && End())
	{
		m_preview = false;
		OnStart();
	}
}
void ClearWave::OnFinish()
{
	KuroEngine::Instance()->ChangeScene("OutGame");
}

std::shared_ptr<Camera> ClearWave::GetSubCam()
{
	return m_referGameCam.lock()->GetSubCam();
}

void ClearWave::OnImguiItems()
{
	if (ImGui::Checkbox("Debug", &m_debug))
	{
		if (m_debug)this->Start();
	}
	if (ImGui::Button("Preview"))
	{
		m_preview = true;
		this->Start();
	}
}

ClearWave::ClearWave() : Event(false, false), Debugger("ClearWave")
{
	m_cam = std::make_shared<Camera>("ClearWave - Event - Camera");
}
