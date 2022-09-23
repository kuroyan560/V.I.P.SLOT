#pragma once
#include <Windows.h>
#include <tchar.h>
#include <sstream>

#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
class Fps
{
	const int m_frameRate;
	const float m_minFrameTime;
	float m_frameTime = 0;
	LARGE_INTEGER m_timeStart;
	LARGE_INTEGER m_timeEnd;
	LARGE_INTEGER m_timeFreq;
	// fps���擾����Ȃ�0�ŏ��������Ȃ��ƃS�~���������ă}�C�i�X����n�܂����肷��(�����m��Ȃ�)
	float m_fps = 0;

public:
	Fps(const int& FrameRate);
	void Update();

	const float& GetNowFps()const { return m_fps; }
};