#include "GameCamera.h"

GameCamera::GameCamera()
{
	m_cam = std::make_shared<Camera>("GameCamera");
}

void GameCamera::Init()
{
	//デフォルトの位置と視野角で初期化
	m_cam->SetAngleOfView(Angle(m_defaultCapeView));
	m_cam->SetPos(m_defaultPos);

	//中央を向く
	m_cam->SetTarget({ 0.0f,m_defaultPos.y + 2.0f,0.0f });
}

void GameCamera::Update()
{
}
