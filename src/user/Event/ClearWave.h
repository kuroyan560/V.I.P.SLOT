#pragma once
#include"Event.h"
#include"Timer.h"
#include"Debugger.h"
#include"Vec.h"
#include"KuroMath.h"
#include"MotionWork.h"
class TimeScale;
class GameCamera;
class WaveMgr;
class EnemyEmitter;
class Player;
class ObjectManager;
class CollisionManager;
class StartWave;

//ウェーブクリア時の演出
class ClearWave : public Event, public Debugger
{
	//演出で参照するポインタの保持
	std::weak_ptr<GameCamera>m_referGameCam;
	std::weak_ptr<WaveMgr>m_referWaveMgr;
	std::weak_ptr<EnemyEmitter>m_referEnemyEmitter;
	std::weak_ptr<Player>m_referPlayer;
	std::weak_ptr<ObjectManager>m_referObjMgr;
	std::weak_ptr<CollisionManager>m_referCollisionMgr;
	std::weak_ptr<StartWave>m_referStartWaveEvent;
	TimeScale* m_referTimeScale;

	//演出ステータス
	enum STATUS { START_WAIT, WAIT, CAM_WORK, END_WAIT, END }m_status;

	//待機時間計測
	Timer m_waitTimer;

	//停止からスローモーションに映る際のイージングパラメータ
	EasingParameter m_slowEaseParam = { InOut,Circ };
	//スローモーションタイムスケール
	float m_slowTimeScale = 0.7f;

	//カメラ
	std::shared_ptr<Camera>m_cam;

	//カメラワーク
	MotionWork3D m_camWork;

	//演出開始直後の待機時間
	float m_startWaitInterval = 60.0f;
	//カメラワーク間の待機時間
	float m_camWorkWaitInterval = 25.0f;
	//カメラワーク終了後、スローの様子を映す待機時間
	float m_slowWaitInterval = 120.0f;
	//演出終了直後の待機時間
	float m_endWaitInterval = 60.0f;

	bool m_enemyKill;

	bool m_debug = false;
	bool m_preview = false;

	void OnStart()override;
	void OnUpdate()override;
	void OnFinish()override;
	bool End()override { return m_status == END; }
	std::shared_ptr<Camera>GetMainCam()override;
	std::shared_ptr<Camera>GetSubCam()override;

	void OnImguiItems()override;

	//回復キットの放出
	void EmitHealKit(int arg_num);

public:
	ClearWave();

	//参照するパラメータを保持
	void Awake(std::weak_ptr<GameCamera>arg_gameCam,
		std::weak_ptr<WaveMgr>arg_waveMgr,
		std::weak_ptr<EnemyEmitter>arg_enemyEmitter,
		std::weak_ptr<Player>arg_player,
		std::weak_ptr<ObjectManager>arg_objMgr,
		std::weak_ptr<CollisionManager>arg_colMgr,
		std::weak_ptr<StartWave>arg_referStartWaveEvent,
		TimeScale* arg_timeScale)
	{
		m_referGameCam = arg_gameCam; 
		m_referWaveMgr = arg_waveMgr;
		m_referEnemyEmitter = arg_enemyEmitter;
		m_referPlayer = arg_player;
		m_referObjMgr = arg_objMgr;
		m_referCollisionMgr = arg_colMgr;
		m_referStartWaveEvent = arg_referStartWaveEvent;
		m_referTimeScale = arg_timeScale;
	}

	void DebugHealKitEmit(int arg_num)
	{
		EmitHealKit(arg_num);
	}
};

