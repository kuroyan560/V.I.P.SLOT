#include "GameCamera.h"

GameCamera::GameCamera()
{
	m_cam = std::make_shared<Camera>("GameCamera");
}

void GameCamera::Init()
{
	//�f�t�H���g�̈ʒu�Ǝ���p�ŏ�����
	m_cam->SetAngleOfView(Angle(m_defaultCapeView));
	m_cam->SetPos(m_defaultPos);

	//����������
	m_cam->SetTarget({ 0.0f,m_defaultPos.y + 2.0f,0.0f });
}

void GameCamera::Update()
{
}
