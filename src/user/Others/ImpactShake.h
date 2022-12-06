#pragma once
#include"Vec.h"
#include"Timer.h"

//�Ռ��ɂ��h��
class ImpactShake
{
	//�A�N�e�B�u�t���O
	bool m_active = false;
	//�U���̑����ԃ^�C�}�[
	Timer m_activeTimer;
	//�U���̃X�p���v���^�C�}�[
	Timer m_spanTimer;
	//�U���̋�����������
	float m_powerMin;
	//�U���̋����������
	float m_powerMax;
	//�U���I�t�Z�b�g
	Vec3<float>m_offset;

public:
	void Init()
	{
		m_offset = { 0,0,0 };
		m_activeTimer.Reset(0);
		m_active = false;
	}
	void Update(float arg_timeScale);

	//�I�t�Z�b�g�Q�b�^
	const Vec3<float>& GetOffset()const { return m_offset; }

	//�U��
	void Shake(float arg_time, float arg_span, float arg_powerMin, float arg_powerMax);

	//�A�N�e�B�u�t���O�Q�b�^
	const bool& IsActive()const { return m_active; }
};

