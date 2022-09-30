#pragma once
#include<array>
#include"Camera.h"
#include<memory>
#include"Timer.h"

class GameCamera
{
	enum { BACK, FRONT, NUM };

	std::array<std::shared_ptr<Camera>, NUM>m_cam;

	//初期座標
	std::array<Vec3<float>, NUM>m_defaultPos =
	{
		Vec3<float>(0.0f,3.0f,-50.0f),
		Vec3<float>(0.0f,13.4f,-59.0f)
	};

	//注視点の高さオフセット
	std::array<Vec3<float>, NUM>m_targetPos =
	{
		Vec3<float>(0.0f,m_defaultPos[BACK].y + 2.0f,0.0f),
		Vec3<float>(0.0f,m_defaultPos[FRONT].y + 2.0f,0.0f),
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
	}m_shake;

	void SetPosAndTarget(Vec3<float>arg_offset = { 0,0,0 });

public:
	GameCamera();
	void Init();
	void Update(float arg_timeScale);

	//カメラ振動
	void Shake(int arg_time, int arg_span, float arg_powerMin, float arg_powerMax);

	//カメラゲッタ
	std::shared_ptr<Camera>& GetBackCam() { return m_cam[BACK]; }
	std::shared_ptr<Camera>& GetFrontCam() { return m_cam[FRONT]; }
};