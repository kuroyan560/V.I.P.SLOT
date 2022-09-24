#include "GameCamera.h"

GameCamera::GameCamera()
{
	m_backGroundCam = std::make_shared<Camera>("GameCamera - Back");
	m_frontCam = std::make_shared<Camera>("GameCamera - Front");
}

void GameCamera::Init()
{
	//�w�i�J����
	m_backGroundCam->SetAngleOfView(Angle(m_backDefaultCapeView));
	m_backGroundCam->SetPos(m_backDefaultPos);
	m_backGroundCam->SetTarget({ 0.0f,m_backTargetHeight,0.0f });

	//�O�i�J����
	m_frontCam->SetAngleOfView(Angle(m_frontDefaultCapeView));
	m_frontCam->SetPos(m_frontDefaultPos);
	m_frontCam->SetTarget({ 0.0f,m_frontTargetHeight,0.0f });
}

void GameCamera::Update()
{
}
