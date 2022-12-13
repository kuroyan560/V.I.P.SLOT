#pragma once
#include <Windows.h>
#include <tchar.h>
#include <sstream>
#include"Debugger.h"
#include"Singleton.h"

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
class Fps : public Debugger, public Singleton<Fps>
{
	friend class Singleton<Fps>;
	Fps() :Debugger("Fps") {}

	int m_frameRate;
	float m_minFrameTime;
	float m_frameTime = 0;
	LARGE_INTEGER m_timeStart;
	LARGE_INTEGER m_timeEnd;
	LARGE_INTEGER m_timeFreq;
	// fpsを取得するなら0で初期化しないとゴミが混ざってマイナスから始まったりする(かも知れない)
	float m_fps = 0;

public:
	//メインループ前に呼び出し
	void LoopInit(const int& FrameRate);
	void Update();

	const float& GetNowFps()const { return m_fps; }
	void OnImguiItems()override;
};