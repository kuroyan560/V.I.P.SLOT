#pragma once
#include"Event.h"
#include"MotionWork.h"
#include"Timer.h"
#include"HandShake.h"
#include"InGameMonitor.h"
#include"Debugger.h"
class TextureBuffer;
class GameCamera;
class WaveMgr;
class Screen;

//ウェーブスタート時の演出
class StartWave : public Event, public InGameMonitor, public Debugger
{
	enum { APPEAR, WAIT, DISAPPEAR }m_status;

	//演出で参照するポインタの保持
	std::weak_ptr<WaveMgr>m_referWaveMgr;
	std::weak_ptr<Screen>m_referScreen;
	std::weak_ptr<GameCamera>m_referGameCam;

	//スクリーンの動き
	MotionWork3D m_appearScreenWork;
	MotionWork3D m_disappearScreenWork;

	//プレイヤー追従LerpのRate
	float m_followLerpRate = 0.08f;

	//プレイヤーに対しての、デフォルトのスクリーンの位置オフセット
	Vec3<float>m_defaultPosOffset = { 0.0f,13.35f,-7.15f };

	//待機時間
	float m_waitInterval = 120.0f;
	Timer m_waitTimer;

	//敵の出現進行フラグ
	bool m_enemyEmit = false;

	//スクリーンに映すテクスチャデータ
	std::shared_ptr<TextureBuffer>m_goodLuckTex;
	std::array<std::shared_ptr<TextureBuffer>, 10>m_waveNumTex;
	std::shared_ptr<TextureBuffer>m_waveStrTex;
	MotionWork2D m_screenWaveDraw;
	float m_wavePosScrollX = 0.0f;

	void OnStart()override;
	void OnUpdate()override;
	void OnFinish()override;
	bool End()override;

	std::shared_ptr<Camera> GetMainCam()override;
	std::shared_ptr<Camera> GetSubCam()override;

	void OnImguiItems()override;

public:
	StartWave();

	//参照するパラメータを保持
	void Awake(std::weak_ptr<WaveMgr>arg_waveMgr,
		std::weak_ptr<Screen>arg_screen,
		std::weak_ptr<GameCamera>arg_gameCam)
	{
		m_referWaveMgr = arg_waveMgr;
		m_referScreen = arg_screen;
		m_referGameCam = arg_gameCam;
	}

	//敵の出現進行フラグ
	const bool& GetEmitEnemy()const { return m_enemyEmit; }

};