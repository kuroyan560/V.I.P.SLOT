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
	// fps���擾����Ȃ�0�ŏ��������Ȃ��ƃS�~���������ă}�C�i�X����n�܂����肷��(�����m��Ȃ�)
	float m_fps = 0;

public:
	//���C�����[�v�O�ɌĂяo��
	void LoopInit(const int& FrameRate);
	void Update();

	const float& GetNowFps()const { return m_fps; }
	void OnImguiItems()override;
};