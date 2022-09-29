#pragma once
class Timer
{
	//�ݒ莞��
	float m_intervalTime = 0.0f;
	//�o�ߎ���
	float m_elaspedTime = 0.0f;

public:
	Timer(float arg_intervalTime = 1.0f) :m_intervalTime(arg_intervalTime) {}

	//�ݒ莞�Ԃ��o�߂�����
	bool IsTimeUp()const { return m_intervalTime <= m_elaspedTime; }

	//�o�ߎ��� / �ݒ莞��
	float GetTimeRate()const
	{
		if (IsTimeUp())return 1.0f;
		return m_elaspedTime / m_intervalTime;
	}

	//(1.0f - �o�ߎ��� / �ݒ莞��)
	float GetInverseTimeRate()const { return 1.0f - GetTimeRate(); }

	//�c�莞��
	float GetLeftTime()const { return m_intervalTime - m_elaspedTime; }

	//�o�ߎ���
	const float& GetElaspedTime()const { return m_elaspedTime; }

	/// <summary>
	/// ���Ԍo��
	/// </summary>
	/// <param name="arg_timeScale">�^�C���X�P�[��</param>
	/// <returns>�ݒ莞�Ԃ��o�߂�����</returns>
	bool UpdateTimer(float arg_timeScale = 1.0f)
	{
		m_elaspedTime += arg_timeScale;
		return IsTimeUp();
	}

	//���Z�b�g
	void Reset() { m_elaspedTime = 0.0f; }
	void Reset(float arg_interval)
	{
		m_intervalTime = arg_interval;
		Reset();
	}

	void operator=(float arg) = delete;
};