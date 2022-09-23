#pragma once
#include<windows.h>
#include<string>
#include"Vec.h"

class WinApp
{
private:
	static WinApp* s_instance;
public:
	static WinApp* Instance() 
	{ 
		if (s_instance == nullptr)
		{
			printf("WinApp�̃C���X�^���X���Ăяo�����Ƃ��܂�����nullptr�ł���\n");
			assert(0);
		}
		return s_instance; 
	}

private:
	Vec2<int>m_winSize;
	Vec2<float>m_winDifferRate = { 1.0f,1.0f };	//�t���X�N���[���ɂ����ۂ̃E�B���h�E�T�C�Y�{��
	WNDCLASSEX m_wc{};
	HWND m_hwnd;

	void Initialize(const std::string& WinName, const Vec2<int>WinSize, const bool& FullScreen, const wchar_t* IconPath);

public:
	WinApp(const std::string& WinName, const Vec2<int>WinSize, const bool& FullScreen, const wchar_t* IconPath = nullptr)
	{
		if (s_instance != nullptr)
		{
			printf("���ɃC���X�^���X������܂��BWinApp�͂P�̃C���X�^���X�������Ă܂���\n");
			assert(0);
		}
		s_instance = this;
		Initialize(WinName, WinSize, FullScreen, IconPath);
	}
	const HWND& GetHwnd() { return m_hwnd; }
	const WNDCLASSEX& GetWinClass() { return m_wc; }
	const Vec2<int>& GetWinSize() { return m_winSize; }
	const Vec2<int>& GetWinCenter() { return m_winSize / 2; }
	Vec2<float>GetExpandWinSize() { return m_winSize.Float() * m_winDifferRate; }	//�E�B���h�E�T�C�Y�{�������������T�C�Y
	Vec2<float>GetExpandWinCenter() { return GetExpandWinSize() / 2.0f; }
	const Vec2<float>& GetWinDifferRate() { return m_winDifferRate; }
	float GetAspect()
	{
		auto size = GetExpandWinSize();
		return size.x / size.y;
	}
};

