#pragma once
class Timer
{
	//�ݒ莞��
	float m_intervalTime = 0.0f;
	//�o�ߎ���
	float m_elaspedTime = 0.0f;
	//�^�C���A�b�v�g���K�[�L�^
	bool m_onTimeUpTrigger = false;

public:
	Timer(float arg_intervalTime = 1.0f) :m_intervalTime(arg_intervalTime) {}

	//�ݒ莞�Ԃ��o�߂�����
	bool IsTimeUp()const { return m_intervalTime <= m_elaspedTime; }

	//�^�C���A�b�v�����u��
	const bool& IsTimeUpOnTrigger()const { return m_onTimeUpTrigger; }

	//�o�ߎ��� / �ݒ莞��
	float GetTimeRate()const
	{
		if (IsTimeUp())return 1.0f;
		return m_elaspedTime / m_intervalTime;
	}

	//�^�C�}�[�X�^�[�g�����u��
	bool IsTimeStartOnTrigger()const { return GetTimeRate() == 0.0f; }

	//(1.0f - �o�ߎ��� / �ݒ莞��)
	float GetInverseTimeRate()const { return 1.0f - GetTimeRate(); }

	//�c�莞��
	float GetLeftTime()const 
	{
		if (IsTimeUp())return 0.0f;
		return m_intervalTime - m_elaspedTime; 
	}

	//�o�ߎ���
	const float& GetElaspedTime()const { return m_elaspedTime; }

	//�ݒ莞��
	const float& GetIntervalTime()const { return m_intervalTime; }

	/// <summary>
	/// ���Ԍo��
	/// </summary>
	/// <param name="arg_timeScale">�^�C���X�P�[��</param>
	/// <returns>�ݒ莞�Ԃ��o�߂�����</returns>
	bool UpdateTimer(float arg_timeScale = 1.0f)
	{
		//�O�t���[���Ń^�C���A�b�v������
		bool oldTimeUp = IsTimeUp();
		m_elaspedTime += arg_timeScale;

		//�^�C���A�b�v�̃g���K�[�L�^
		m_onTimeUpTrigger = !oldTimeUp && IsTimeUp();

		return IsTimeUp();
	}

	//�����I�Ɏ��Ԍo�ߊ���
	void ForciblyTimeUp()
	{
		m_elaspedTime = m_intervalTime;
	}

	//���Z�b�g
	void Reset() { m_elaspedTime = 0.0f; }
	void Reset(float arg_interval)
	{
		m_intervalTime = arg_interval;
		Reset();
	}
	void Reset(int arg_interval)
	{
		Reset(static_cast<float>(arg_interval));
	}

	void operator=(float arg) = delete;
};