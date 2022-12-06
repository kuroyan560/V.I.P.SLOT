#include "ClearWave.h"
#include"TimeScale.h"
#include"KuroMath.h"
#include"KuroEngine.h"
#include"Camera.h"
#include"GameCamera.h"
#include"Player.h"
#include"WaveMgr.h"
#include"HUDInterface.h"
#include"EnemyEmitter.h"

void ClearWave::OnStart()
{
	m_status = WAIT;

	m_timer.Reset(m_startStopWaitInterval);
	*m_cam = *m_referGameCam.lock()->GetMainCam();

	m_camWorkIdx = -1;

	m_referTimeScale->Set(0.0f);

	//HUD非表示
	HUDInterface::s_draw = false;
}

void ClearWave::UpdateCameraWork()
{
	m_camWorkIdx++;
	m_timer.Reset(m_camWorkInterval);

	//最後のカメラワーク時
	if (static_cast<int>(m_camWorks.size() - 1) <= m_camWorkIdx)
	{
		m_timer.Reset(m_slowWaitInterval);
		//敵全員死亡
		m_referEnemyEmitter.lock()->KillAllEnemys();
	}
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
			UpdateCameraWork();
		}
	}
	else if (m_status == CAM_WORK)
	{
		//カメラワーク
		if (0 <= m_camWorkIdx && m_camWorkIdx < static_cast<int>(m_camWorks.size()))
		{
			//現在のカメラワークの参照取得
			const auto& camWork = m_camWorks[m_camWorkIdx];

			//プレイヤー座標取得
			auto playerPos = InGameMonitor::GetPlayer()->GetCenterPos();

			//座標
			Vec3<float>pos = camWork.m_easeParam.Calculate(m_timer.GetTimeRate(),
				playerPos + camWork.m_startPosOffset,
				playerPos + camWork.m_endPosOffset);
			m_cam->SetPos(pos);

			//注視点
			Vec3<float>target = camWork.m_easeParam.Calculate(m_timer.GetTimeRate(),
				playerPos + camWork.m_startTargetOffset,
				playerPos + camWork.m_endTargetOffset);
			m_cam->SetTarget(target);
		}

		//最後のカメラワークでスローモーションに
		if (static_cast<int>(m_camWorks.size()) - 1 <= m_camWorkIdx)
		{
			float timeScale = m_slowEaseParam.Calculate(m_timer.GetTimeRate(), 0.0f, m_slowTimeScale);
			m_referTimeScale->Set(timeScale);
		}

		//カメラワーク進行
		if (m_timer.IsTimeUp())
		{
			//終了
			if (static_cast<int>(m_camWorks.size()) - 1 <= m_camWorkIdx)
			{
				m_status = END;
				m_referWaveMgr.lock()->ProceedWave();
				*m_referGameCam.lock()->GetMainCam() = *this->m_cam;
				m_referTimeScale->Set(1.0f);
			}
			//カメラワーク間の待機時間
			else
			{
				m_status = WAIT;
				m_timer.Reset(m_camWorkWaitInterval);
			}
		}
	}

	//デバッグモード
	if (m_debug && End())
	{
		m_preview = false;
		OnStart();
	}
}
void ClearWave::OnFinish()
{
	//HUD表示
	HUDInterface::s_draw = true;
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
		else
		{
			m_referTimeScale->Set(1.0f);
		}
	}

	if (ImGui::Button("Preview"))
	{
		m_preview = true;
		this->Start();
	}

	if (!m_debug)return;

	ImGui::Separator();
	ImGui::DragFloat("StartStopWaitInterval", &m_startStopWaitInterval, 0.5f, 1.0f);
	ImGui::DragFloat("CamWorkInterval", &m_camWorkInterval, 0.5f, 1.0f);
	ImGui::DragFloat("CamWorkWaitInterval", &m_camWorkWaitInterval, 0.5f, 1.0f);
	ImGui::DragFloat("SlowWaitInterval", &m_slowWaitInterval, 0.5f, 1.0f);

	m_slowEaseParam.ImguiDebug("Slow");
	ImGui::DragFloat("SlowTimeScale", &m_slowTimeScale, 0.0f, 0.0f);

	int eraseIdx = -1;
	ImGui::BeginChild("CamWorkPosOffset", ImVec2(250, 220),ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < static_cast<int>(m_camWorks.size()); ++i)
	{
		std::string label = std::to_string(i);

		if (ImGui::TreeNode(("CameraWork - " + label).c_str()))
		{
			bool start = false, end = false;
			if (ImGui::DragFloat3("StartPos", (float*)&m_camWorks[i].m_startPosOffset, 0.02f))start = true;
			if (ImGui::DragFloat3("EndPos", (float*)&m_camWorks[i].m_endPosOffset, 0.02f))end = true;
			if (ImGui::DragFloat3("StartTarget", (float*)&m_camWorks[i].m_startTargetOffset, 0.02f))start = true;
			if (ImGui::DragFloat3("EndTarget", (float*)&m_camWorks[i].m_endTargetOffset, 0.02f))end = true;

			auto playerPos = InGameMonitor::GetPlayer()->GetCenterPos();
			if (start)
			{
				m_cam->SetPos(playerPos + m_camWorks[i].m_startPosOffset);
				m_cam->SetTarget(playerPos + m_camWorks[i].m_startTargetOffset);
			}
			if (end)
			{
				m_cam->SetPos(playerPos + m_camWorks[i].m_endPosOffset);
				m_cam->SetTarget(playerPos + m_camWorks[i].m_endTargetOffset);
			}

			m_camWorks[i].m_easeParam.ImguiDebug("CameraWork - " + label);

			ImGui::TreePop();
		}
		ImGui::SameLine();
		if (ImGui::Button((label + "-").c_str()))
		{
			eraseIdx = i;
		}

	}
	ImGui::EndChild();

	if (eraseIdx != -1)m_camWorks.erase(m_camWorks.begin() + eraseIdx);

	if (ImGui::Button("+"))m_camWorks.emplace_back();

}

ClearWave::ClearWave() : Event(false), Debugger("ClearWave")
{
	m_cam = std::make_shared<Camera>("ClearWave - Event - Camera");
	m_camWorks.emplace_back();
	m_camWorks.back().m_startPosOffset = { -8.62f, 4.76f, -19.96f };
	m_camWorks.back().m_endPosOffset = { 4.16f, 2.1f, -10.02f };

	m_camWorks.emplace_back();
	m_camWorks.back().m_startPosOffset = { 13.4f, 4.0f, -12.94f };
	m_camWorks.back().m_endPosOffset = { -3.52f, 2.28f, -5.82f };

	m_camWorks.emplace_back();
	m_camWorks.back().m_startPosOffset = { 0.0f, 0.62f, -3.0f };
	m_camWorks.back().m_endPosOffset = { 0.0f, 9.7f, -32.66f };
	m_camWorks.back().m_startTargetOffset = { 0.0f, 0.36f, 0.0f };
	m_camWorks.back().m_endTargetOffset = { 0.0f, 9.3f, 0.0f };
	m_camWorks.back().m_easeParam.m_easeType = Quint;
}
