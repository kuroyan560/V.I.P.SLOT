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

//�E�F�[�u�N���A���̉��o
class ClearWave : public Event, public Debugger
{
	//���o�ŎQ�Ƃ���|�C���^�̕ێ�
	std::weak_ptr<GameCamera>m_referGameCam;
	std::weak_ptr<WaveMgr>m_referWaveMgr;
	std::weak_ptr<EnemyEmitter>m_referEnemyEmitter;
	std::weak_ptr<Player>m_referPlayer;
	std::weak_ptr<ObjectManager>m_referObjMgr;
	std::weak_ptr<CollisionManager>m_referCollisionMgr;
	TimeScale* m_referTimeScale;

	//���o�X�e�[�^�X
	enum STATUS { WAIT, CAM_WORK, END }m_status;

	//���Ԍv��
	Timer m_timer;

	//�J�������[�N���
	struct CameraWork
	{
		//�J�������W�J�n�I�t�Z�b�g
		Vec3<float>m_startPosOffset;
		//�J�������W�I���I�t�Z�b�g
		Vec3<float>m_endPosOffset;
		//�J�����^�[�Q�b�g���W�J�n�I�t�Z�b�g
		Vec3<float>m_startTargetOffset = { 0,0,0 };
		//�J�����^�[�Q�b�g���W�I���I�t�Z�b�g
		Vec3<float>m_endTargetOffset = { 0,0,0 };

		//�J�������W�ړ��̃C�[�W���O�p�����[�^
		EasingParameter m_easeParam = { Out,Exp };
	};
	//�J�������[�N�z��
	std::vector<CameraWork>m_camWorks;
	//���ݐi�s���̃J�������[�N�C���f�b�N�X
	int m_camWorkIdx;

	//��~����X���[���[�V�����ɉf��ۂ̃C�[�W���O�p�����[�^
	EasingParameter m_slowEaseParam = { InOut,Circ };
	//�X���[���[�V�����^�C���X�P�[��
	float m_slowTimeScale = 0.7f;

	//�J����
	std::shared_ptr<Camera>m_cam;

	//���o�J�n����̑ҋ@����
	float m_startStopWaitInterval = 60.0f;
	//�J�������[�N�ɂ����鎞��
	float m_camWorkInterval = 25.0f;
	//�J�������[�N�Ԃ̑ҋ@����
	float m_camWorkWaitInterval = 25.0f;
	//�J�������[�N�I����A�X���[�̗l�q���f���ҋ@����
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

	//�񕜃L�b�g�̕��o
	void EmitHealKit(int arg_num);

public:
	ClearWave();

	//�Q�Ƃ���p�����[�^��ێ�
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

