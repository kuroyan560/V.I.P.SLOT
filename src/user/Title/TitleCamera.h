#pragma once
#include<memory>
class Camera;
#include"HandShake.h"
#include"Debugger.h"
#include"MotionWork.h"

class TitleCamera : public Debugger
{
	std::shared_ptr<Camera>m_cam;
	HandShake m_handShake;

	MotionWork3D m_motion;
	bool m_motionStart = false;
	Timer m_waitTimer;

	void OnImguiItems()override;

public:
	TitleCamera();
	void Init();
	void Update(float arg_timeScale);
	void StartMotion(float arg_waitInterval);

	operator std::shared_ptr<Camera>& ()
	{
		return m_cam;
	}

	operator Camera& ()
	{
		return *m_cam;
	}
};

