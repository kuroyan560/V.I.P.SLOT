#pragma once
#include<array>
#include"Camera.h"
#include<memory>
#include"Timer.h"

class GameCamera
{
	enum { BACK, FRONT, NUM };

	std::array<std::shared_ptr<Camera>, NUM>m_cam;

	//�������W
	std::array<Vec3<float>, NUM>m_defaultPos =
	{
		Vec3<float>(0.0f,3.0f,-50.0f),
		Vec3<float>(0.0f,13.4f,-59.0f)
	};

	//�����_�̍����I�t�Z�b�g
	std::array<Vec3<float>, NUM>m_targetPos =
	{
		Vec3<float>(0.0f,m_defaultPos[BACK].y + 2.0f,0.0f),
		Vec3<float>(0.0f,m_defaultPos[FRONT].y + 2.0f,0.0f),
	};

	//����p
	std::array<int, NUM>m_defaultCapeView =
	{
		45,60
	};

	//�U�����
	struct Shake
	{
		//�U���̑����ԃ^�C�}�[
		Timer m_activeTimer;
		//�U���̃X�p���v���^�C�}�[
		Timer m_spanTimer;
		//�U���̃X�p��
		int m_span;
		//�U���̋�����������
		float m_powerMin;
		//�U���̋����������
		float m_powerMax;
	}m_shake;

	void SetPosAndTarget(Vec3<float>arg_offset = { 0,0,0 });

public:
	GameCamera();
	void Init();
	void Update(float arg_timeScale);

	//�J�����U��
	void Shake(int arg_time, int arg_span, float arg_powerMin, float arg_powerMax);

	//�J�����Q�b�^
	std::shared_ptr<Camera>& GetBackCam() { return m_cam[BACK]; }
	std::shared_ptr<Camera>& GetFrontCam() { return m_cam[FRONT]; }
};