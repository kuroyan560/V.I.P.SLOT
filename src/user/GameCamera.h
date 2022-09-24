#pragma once
#include"Camera.h"
#include<memory>

class GameCamera
{
	std::shared_ptr<Camera>m_cam;

	//À•W
	Vec3<float>m_defaultPos = { 0.0f,6.0f,-50.0f };

	//‹–ìŠp
	int m_defaultCapeView = 45;

public:
	GameCamera();
	void Init();
	void Update();

	std::shared_ptr<Camera>& GetCam() { return m_cam; }
};