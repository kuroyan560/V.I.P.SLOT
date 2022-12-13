#pragma once
#include"Event.h"
#include"Timer.h"
#include"Debugger.h"
#include"Vec.h"
#include"KuroMath.h"
class TimeScale;
class GameCamera;
class WaveMgr;
class EnemyEmitter;
class Player;
class ObjectManager;
class CollisionManager;

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
	TimeScale* m_referTimeScale;

	//演出ステータス
	enum STATUS { WAIT, CAM_WORK, END }m_status;

	//時間計測
	Timer m_timer;

	//カメラワーク情報
	struct CameraWork
	{
		//カメラ座標開始オフセット
		Vec3<float>m_startPosOffset;
		//カメラ座標終了オフセット
		Vec3<float>m_endPosOffset;
		//カメラターゲット座標開始オフセット
		Vec3<float>m_startTargetOffset = { 0,0,0 };
		//カメラターゲット座標終了オフセット
		Vec3<float>m_endTargetOffset = { 0,0,0 };

		//カメラ座標移動のイージングパラメータ
		EasingParameter m_easeParam = { Out,Exp };
	};
	//カメラワーク配列
	std::vector<CameraWork>m_camWorks;
	//現在進行中のカメラワークインデックス
	int m_camWorkIdx;

	//停止からスローモーションに映る際のイージングパラメータ
	EasingParameter m_slowEaseParam = { InOut,Circ };
	//スローモーションタイムスケール
	float m_slowTimeScale = 0.7f;

	//カメラ
	std::shared_ptr<Camera>m_cam;

	//演出開始直後の待機時間
	float m_startStopWaitInterval = 60.0f;
	//カメラワークにかかる時間
	float m_camWorkInterval = 25.0f;
	//カメラワーク間の待機時間
	float m_camWorkWaitInterval = 25.0f;
	//カメラワーク終了後、スローの様子を映す待機時間
	float m_slowWaitInterval = 120.0f;

	bool m_debug = false;
	bool m_preview = false;

	void OnStart()override;
	void OnUpdate()override;
	void OnFinish()override;
	bool End()override { return m_status == END; }
	std::shared_ptr<Camera>GetMainCam()override { return m_cam; }
	std::shared_ptr<Camera>GetSubCam()override;

	void OnImguiItems()override;

	void UpdateCameraWork();

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
		TimeScale* arg_timeScale)
	{
		m_referGameCam = arg_gameCam; 
		m_referWaveMgr = arg_waveMgr;
		m_referEnemyEmitter = arg_enemyEmitter;
		m_referPlayer = arg_player;
		m_referObjMgr = arg_objMgr;
		m_referCollisionMgr = arg_colMgr;
		m_referTimeScale = arg_timeScale;
	}

	void DebugHealKitEmit(int arg_num)
	{
		EmitHealKit(arg_num);
	}
};

