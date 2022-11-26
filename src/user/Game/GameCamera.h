#pragma once
#include<array>
#include"Camera.h"
#include<memory>
#include"Timer.h"

class GameCamera
{
	enum { SUB, MAIN, NUM };

	std::array<std::shared_ptr<Camera>, NUM>m_cam;

	//初期座標
	std::array<Vec3<float>, NUM>m_defaultPos =
	{
		Vec3<float>(0.0f,3.0f,-50.0f),
		Vec3<float>(0.0f,10.4f,-59.0f)
	};

	//注視点の高さオフセット
	std::array<Vec3<float>, NUM>m_targetPos =
	{
		Vec3<float>(0.0f,m_defaultPos[SUB].y + 2.0f,0.0f),
		Vec3<float>(0.0f,m_defaultPos[MAIN].y + 2.0f,0.0f),
	};

	//視野角
	std::array<int, NUM>m_defaultCapeView =
	{
		45,60
	};

	//振動情報
	struct Shake
	{
		//振動の総時間タイマー
		Timer m_activeTimer;
		//振動のスパン計測タイマー
		Timer m_spanTimer;
		//振動のスパン
		int m_span;
		//振動の強さ乱数下限
		float m_powerMin;
		//振動の強さ乱数上限
		float m_powerMax;
		//振動オフセット
		Vec3<float>m_offset;

		void Init()
		{
			m_activeTimer.Reset(0);
		}
	}m_shake;

	//Lerpの目標値
	Vec3<float>m_posLerpOffset;
	Vec3<float>m_targetLerpOffset;

	void SetPosAndTarget(Vec3<float>arg_absOffset, Vec3<float>arg_lerpOffset, float arg_timeScale);

public:
	GameCamera();
	void Init();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="arg_timeScale">タイムスケール</param>
	/// <param name="arg_playersDisplacement">プレイヤーの初期位置からの変位</param>
	void Update(float arg_timeScale, Vec3<float>arg_playersDisplacement);

	//カメラ振動
	void Shake(int arg_time, int arg_span, float arg_powerMin, float arg_powerMax);

	//カメラゲッタ
	std::shared_ptr<Camera>& GetSubCam() { return m_cam[SUB]; }
	std::shared_ptr<Camera>& GetMainCam() { return m_cam[MAIN]; }
};