#pragma once
#include"Event.h"
#include"Timer.h"
class TimeScale;

//�E�F�[�u�N���A���̉��o
class ClearWave : public Event
{
	Timer m_focusTimer;
	Timer m_waitTimer;

	void OnStart()override;
	void OnUpdate(TimeScale& arg_timeScale)override;
	void OnFinish()override;
	bool End()override { return m_waitTimer.IsTimeUp(); }

public:
	ClearWave() :Event(false) {}
};

