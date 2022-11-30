#include "TitleCamera.h"
#include"Camera.h"

void TitleCamera::OnImguiItems()
{
	ImGui::Text("TimeRate : { %f }", m_handShake.GetTimeRate());
	ImGui::Separator();
	if (ImGui::DragInt("Interval", &m_handShake.m_interval))m_handShake.Init();
	ImGui::DragFloat("ValX", &m_handShake.m_val.x);
	ImGui::DragFloat("ValY", &m_handShake.m_val.y);
}

TitleCamera::TitleCamera() : Debugger("TitleCamera - HandShake")
{
	m_cam = std::make_shared<Camera>("TitleCamera");
}

void TitleCamera::Init()
{
	m_handShake.Init();
}

void TitleCamera::Update(float arg_timeScale)
{
	m_handShake.Update(arg_timeScale, KuroMath::RotateMat(Vec3<float>(0, 0, 1), m_cam->GetForward()));
	m_cam->SetPos(Vec3<float>(0, 2, -10.0f) + m_handShake.GetOffset());
	m_cam->SetTarget(m_cam->GetPos() + Vec3<float>(0, 0, 1));
}
