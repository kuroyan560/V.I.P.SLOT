#pragma once
#include<array>
#include"Camera.h"
#include<memory>
#include"Timer.h"
#include"ImpactShake.h"

class GameCamera
{
	enum { SUB, MAIN, NUM };

	std::array<std::shared_ptr<Camera>, NUM>m_cam;

	//�������W
	std::array<Vec3<float>, NUM>m_defaultPos =
	{
		Vec3<float>(0.0f,3.0f,-50.0f),
		Vec3<float>(0.0f,10.4f,-59.0f)
	};

	//�����_�̍����I�t�Z�b�g
	std::array<Vec3<float>, NUM>m_targetPos =
	{
		Vec3<float>(0.0f,m_defaultPos[SUB].y + 2.0f,0.0f),
		Vec3<float>(0.0f,m_defaultPos[MAIN].y + 2.0f,0.0f),
	};

	//����p
	std::array<int, NUM>m_defaultCapeView =
	{
		45,60
	};

	//1�t���[���O�̐U���I�t�Z�b�g
	Vec3<float>m_oldShakeOffset;

	//Lerp�̖ڕW�l
	Vec3<float>m_posLerpOffset;
	Vec3<float>m_targetLerpOffset;

	//�Ռ��ɂ��J�����U��
	ImpactShake m_shake;

	void SetPosAndTarget(Vec3<float>arg_lerpOffset, float arg_timeScale);

public:
	GameCamera();
	void Init();

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="arg_timeScale">�^�C���X�P�[��</param>
	/// <param name="arg_playersDisplacement">�v���C���[�̏����ʒu����̕ψ�</param>
	void Update(float arg_timeScale, Vec3<float>arg_playersDisplacement);

	//�J�����U��
	void Shake(float arg_time, float arg_span, float arg_powerMin, float arg_powerMax)
	{
		m_shake.Shake(arg_time, arg_span, arg_powerMin, arg_powerMax);
	}

	//�J�����Q�b�^
	std::shared_ptr<Camera>& GetSubCam() { return m_cam[SUB]; }
	std::shared_ptr<Camera>& GetMainCam() { return m_cam[MAIN]; }
};