#include "HandShake.h"
#include"KuroFunc.h"
#include"PerlinNoise.h"

void HandShake::Init()
{
	m_timer.Reset(m_interval);
}

void HandShake::Update(float arg_timeScale,const Matrix& arg_rotate)
{
	if (m_timer.UpdateTimer(arg_timeScale))
	{
		m_timer.Reset(m_interval);
		m_seed.x = KuroFunc::GetRand(SEED_MIN, SEED_MAX);
		m_seed.y = KuroFunc::GetRand(SEED_MIN, SEED_MAX);
	}

	//ŽèƒuƒŒ
	Vec3<float>handShake = { 1.0f,1.0f,0.0f };
	handShake.x = PerlinNoise::GetRand(m_timer.GetTimeRate(), 0.0f, m_seed.x) * m_val.x;
	handShake.y = PerlinNoise::GetRand(0.0f, m_timer.GetTimeRate(), m_seed.y) * m_val.y;

	m_offset = KuroMath::TransformVec3(handShake, arg_rotate);
}
