#pragma once
#include<cfloat>
//�ŏ��l�ƍő�l���i�[���邽�߂̍\����
struct ValueMinMax
{
	float m_min;
	float m_max;

	//�����̒l�����߂�
	float GetCenter()const
	{
		return (m_max - m_min) * 0.5f + m_min;
	}

	//�f�t�H���g�����F��r���ɕK���ʂ�悤��
	void Set(const float& Min = FLT_MAX, const float& Max = 0.0f)
	{
		m_min = Min;
		m_max = Max;
	}

	//min��max�̑召�֌W�ُ̈���m�F
	bool Invalid()const
	{
		return m_max < m_min;
	}
	operator bool()const { return !Invalid(); }
};

