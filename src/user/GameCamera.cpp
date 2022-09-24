#include "GameCamera.h"

GameCamera::GameCamera()
{
	m_backGroundCam = std::make_shared<Camera>("GameCamera - Back");
	m_frontCam = std::make_shared<Camera>("GameCamera - Front");
}

void GameCamera::Init()
{
	//デフォルトの位置と視野角で初期化
	m_backGroundCam->SetAngleOfView(Angle(m_defaultCapeView));
	m_backGroundCam->SetPos(m_defaultPos);

	//中央を向く
	m_backGroundCam->SetTarget({ 0.0f,m_defaultPos.y + 2.0f,0.0f });

}

void GameCamera::Update()
{
	m_frontCam->SetAngleOfView(Angle(m_defaultCapeView));
	m_frontCam->SetPos({ 0.0f,6.4f,-59.0f });
	m_frontCam->SetTarget({ 0.0f,m_frontCam->GetPos().y + 7.0f,0.0f});
}
