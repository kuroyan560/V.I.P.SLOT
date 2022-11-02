#pragma once
#include"Vec.h"
#include"Timer.h"
#include"KuroMath.h"

//ŽèƒuƒŒ
class HandShake
{
	static const int SEED_MIN = 0;
	static const int SEED_MAX = 100;
	Vec3<float>m_offset = { 0,0,0 };
	Timer m_timer;
	Vec2<int>m_seed = { 0,1 };

public:
	int m_interval = 300;
	Vec2<float>m_val = { 1.0f,1.0f };

	HandShake() {}
	void Init();
	void Update(float arg_timeScale,const Matrix& arg_rotate);

	const Vec3<float>& GetOffset()const { return m_offset; }
	const float& GetTimeRate()const { return m_timer.GetTimeRate(); }
};

