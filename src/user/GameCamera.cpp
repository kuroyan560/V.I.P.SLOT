#include "GameCamera.h"

GameCamera::GameCamera()
{
	m_cam = std::make_shared<Camera>("GameCamera");
}

void GameCamera::Init()
{
	m_cam->SetAngleOfView(Angle(m_defaultCapeView));
	m_cam->SetPos(m_defaultPos);
	m_cam->SetTarget({ 0.0f,m_defaultPos.y,0.0f });
}

void GameCamera::Update()
{
}
