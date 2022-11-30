#pragma once
#include"Event.h"
#include"Timer.h"
class TimeScale;
class GameCamera;
class Player;

//ウェーブクリア時の演出
class ClearWave : public Event
{
	std::weak_ptr<GameCamera>m_gameCam;
	std::weak_ptr<Player>m_player;

	Timer m_focusTimer;
	Timer m_waitTimer;
	std::shared_ptr<Camera>m_cam;

	void OnStart()override;
	void OnUpdate(TimeScale& arg_timeScale)override;
	void OnFinish()override;
	bool End()override { return m_waitTimer.IsTimeUp(); }
	std::shared_ptr<Camera>GetMainCam()override { return m_cam; }
	std::shared_ptr<Camera>GetSubCam()override;

public:
	ClearWave();

	//ゲームカメラのポインタを保持
	void Init(	std::weak_ptr<GameCamera>arg_gameCam,
		std::weak_ptr<Player>arg_player) 
	{
		m_gameCam = arg_gameCam; 
		m_player = arg_player;
	}
};

