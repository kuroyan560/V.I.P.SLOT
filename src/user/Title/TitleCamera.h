#pragma once
#include<memory>
class Camera;
#include"HandShake.h"
#include"Debugger.h"

class TitleCamera : public Debugger
{
	std::shared_ptr<Camera>m_cam;
	HandShake m_handShake;

	void OnImguiItems()override;

public:
	TitleCamera();
	void Init();
	void Update(float arg_timeScale);

	operator std::shared_ptr<Camera>& ()
	{
		return m_cam;
	}

	operator Camera& ()
	{
		return *m_cam;
	}
};

