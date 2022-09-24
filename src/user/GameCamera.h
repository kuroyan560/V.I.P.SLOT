#pragma once
#include"Camera.h"
#include<memory>

class GameCamera
{
	std::shared_ptr<Camera>m_backGroundCam;
	std::shared_ptr<Camera>m_frontCam;

	//初期座標
	Vec3<float>m_backDefaultPos = { 0.0f,3.0f,-50.0f };
	Vec3<float>m_frontDefaultPos = { 0.0f,13.4f,-59.0f };

	//注視点の高さオフセット
	float m_backTargetHeight = m_backDefaultPos.y + 2.0f;
	float m_frontTargetHeight = m_frontDefaultPos.y + 2.0f;

	//視野角
	int m_backDefaultCapeView = 45;
	int m_frontDefaultCapeView = 60;

public:
	GameCamera();
	void Init();
	void Update();

	std::shared_ptr<Camera>& GetBackCam() { return m_backGroundCam; }
	std::shared_ptr<Camera>& GetFrontCam() { return m_frontCam; }
};