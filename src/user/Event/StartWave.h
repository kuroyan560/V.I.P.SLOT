#pragma once
#include"Event.h"
#include"MotionWork.h"
#include"Timer.h"
#include"HandShake.h"
#include"InGameMonitor.h"
#include"Debugger.h"
class GameCamera;
class WaveMgr;
class Screen;

//�E�F�[�u�X�^�[�g���̉��o
class StartWave : public Event, public InGameMonitor, public Debugger
{
	enum { APPEAR, WAIT, DISAPPEAR }m_status;

	//���o�ŎQ�Ƃ���|�C���^�̕ێ�
	std::weak_ptr<WaveMgr>m_referWaveMgr;
	std::weak_ptr<Screen>m_referScreen;
	std::weak_ptr<GameCamera>m_referGameCam;

	//�X�N���[���̓���
	MotionWork m_appearScreenWork;
	MotionWork m_disappearScreenWork;

	//�v���C���[�Ǐ]Lerp��Rate
	float m_followLerpRate = 0.08f;

	//�v���C���[�ɑ΂��ẮA�f�t�H���g�̃X�N���[���̈ʒu�I�t�Z�b�g
	Vec3<float>m_defaultPosOffset = { 0.0f,13.35f,-7.15f };

	//�ҋ@����
	float m_waitInterval = 120.0f;
	Timer m_waitTimer;

	//�X�N���[����Y����]
	float m_ySpinRadianMax = Angle::ROUND() * 2.0f;

	void OnStart()override;
	void OnUpdate()override;
	void OnFinish()override;
	bool End()override;

	std::shared_ptr<Camera> GetMainCam()override;
	std::shared_ptr<Camera> GetSubCam()override;

	void OnImguiItems()override;

public:
	StartWave();

	//�Q�Ƃ���p�����[�^��ێ�
	void Awake(std::weak_ptr<WaveMgr>arg_waveMgr,
		std::weak_ptr<Screen>arg_screen,
		std::weak_ptr<GameCamera>arg_gameCam)
	{
		m_referWaveMgr = arg_waveMgr;
		m_referScreen = arg_screen;
		m_referGameCam = arg_gameCam;
	}
};