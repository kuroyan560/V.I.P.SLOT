#pragma once
class Timer
{
	//設定時間
	float m_intervalTime = 0.0f;
	//経過時間
	float m_elaspedTime = 0.0f;

public:
	Timer(float arg_intervalTime = 1.0f) :m_intervalTime(arg_intervalTime) {}

	//設定時間が経過したか
	bool IsTimeUp()const { return m_intervalTime <= m_elaspedTime; }

	//経過時間 / 設定時間
	float GetTimeRate()const
	{
		if (IsTimeUp())return 1.0f;
		return m_elaspedTime / m_intervalTime;
	}

	//(1.0f - 経過時間 / 設定時間)
	float GetInverseTimeRate()const { return 1.0f - GetTimeRate(); }

	//残り時間
	float GetLeftTime()const { return m_intervalTime - m_elaspedTime; }

	//経過時間
	const float& GetElaspedTime()const { return m_elaspedTime; }

	/// <summary>
	/// 時間経過
	/// </summary>
	/// <param name="arg_timeScale">タイムスケール</param>
	/// <returns>設定時間が経過したか</returns>
	bool UpdateTimer(float arg_timeScale = 1.0f)
	{
		m_elaspedTime += arg_timeScale;
		return IsTimeUp();
	}

	//リセット
	void Reset() { m_elaspedTime = 0.0f; }
	void Reset(float arg_interval)
	{
		m_intervalTime = arg_interval;
		Reset();
	}

	void operator=(float arg) = delete;
};