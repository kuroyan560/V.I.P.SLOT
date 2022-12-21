#include "TitleCamera.h"
#include"Camera.h"
#include"UsersInput.h"

void TitleCamera::OnImguiItems()
{
	const auto camPos = m_cam->GetPos();
	const auto camTarget = m_cam->GetTarget();
	ImGui::Text("Pos : { %.2f , %.2f , %.2f }", camPos.x, camPos.y, camPos.z);
	ImGui::Text("Target : { %.2f , %.2f , %.2f }", camTarget.x, camTarget.y, camTarget.z);

	if (ImGui::TreeNode("HandShake"))
	{
		ImGui::Text("TimeRate : { %f }", m_handShake.GetTimeRate());
		ImGui::Separator();
		if (ImGui::DragFloat("Interval", &m_handShake.m_interval, 0.5f))m_handShake.Init();
		ImGui::DragFloat("ValX", &m_handShake.m_val.x);
		ImGui::DragFloat("ValY", &m_handShake.m_val.y);
		ImGui::TreePop();
	}

	m_motion.ImguiDebugItems("CamMotion");
}

TitleCamera::TitleCamera() : Debugger("TitleCamera")
{
	m_cam = std::make_shared<Camera>("TitleCamera");

	Motion3D motion;
	motion.m_startPos = { -0.62f,2.35f,-5.6f };
	motion.m_endPos = { 0.0f,2.0f,-10.0f };
	motion.m_startTarget = { 3.3f,2.35f,-3.2f };
	motion.m_endTarget = motion.m_endPos + Vec3<float>::GetZAxis();
	motion.m_interval = 120.0f;
	motion.m_easeParam.m_easeType = Quint;
	motion.m_easeParam.m_changeType = InOut;
	m_motion.Add(motion);
}

void TitleCamera::Init()
{
	m_handShake.Init();

	m_motion.Init();

	m_cam->SetPos(m_motion.GetPos());
	m_cam->SetTarget(m_motion.GetTarget());

	m_motionStart = false;
}

void TitleCamera::Update(float arg_timeScale)
{
	if (m_motionStart)m_waitTimer.UpdateTimer(1.0f);

	if (m_waitTimer.IsTimeUpOnTrigger())
	{
		m_motion.Start(true);
	}

	//手ブレ更新
	m_handShake.Update(arg_timeScale, KuroMath::RotateMat(Vec3<float>::GetZAxis(), m_cam->GetForward()));

	//カメラワーク
	m_motion.Update(1.0f);

	//手ブレ適用
	Vec3<float>handShakeOffset = m_handShake.GetOffset() * m_motion.GetEaseRate();
	m_cam->SetPos(m_motion.GetPos() + handShakeOffset);
	m_cam->SetTarget(m_motion.GetTarget() + handShakeOffset);
}

void TitleCamera::StartMotion(float arg_waitInterval)
{
	m_motionStart = true;
	m_waitTimer.Reset(arg_waitInterval);
}
