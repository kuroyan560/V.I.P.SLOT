#pragma once
#include"Event.h"
#include"Timer.h"
#include"Debugger.h"
#include"Vec.h"
class TimeScale;
class GameCamera;
class Player;

//ウェーブクリア時の演出
class ClearWave : public Event, public Debugger
{
	//演出で参照するポインタの保持
	std::weak_ptr<GameCamera>m_referGameCam;
	std::weak_ptr<Player>m_referPlayer;
	TimeScale* m_referTimeScale;

	enum STATUS { WAIT, CAM_WORK, SLOW, NUM }m_status;

	Timer m_timer;
	std::vector<Vec3<float>>m_camWorkPosOffset;
	int m_camWorkIdx;
	std::shared_ptr<Camera>m_cam;

	bool m_debug = false;
	bool m_preview = false;

	void OnStart()override;
	void OnUpdate()override;
	void OnFinish()override;
	bool End()override { return m_status == NUM - 1 && m_timer.IsTimeUp(); }
	std::shared_ptr<Camera>GetMainCam()override { return m_cam; }
	std::shared_ptr<Camera>GetSubCam()override;

	void OnImguiItems()override;

	void UpdateCameraWork();

public:
	ClearWave();

	//参照するパラメータを保持
	void Init(	std::weak_ptr<GameCamera>arg_gameCam,
		std::weak_ptr<Player>arg_player,
		TimeScale* arg_timeScale) 
	{
		m_referGameCam = arg_gameCam; 
		m_referPlayer = arg_player;
		m_referTimeScale = arg_timeScale;
	}
};

