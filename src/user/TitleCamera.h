#pragma once
#include<memory>
class Camera;
#include"HandShake.h"

class TitleCamera
{
	std::shared_ptr<Camera>m_cam;
	HandShake m_handShake;

public:
	TitleCamera();
	void Init();
	void Update(float arg_timeScale);

	void ImguiDebug();

	operator std::shared_ptr<Camera>& ()
	{
		return m_cam;
	}

	operator Camera& ()
	{
		return *m_cam;
	}
};

