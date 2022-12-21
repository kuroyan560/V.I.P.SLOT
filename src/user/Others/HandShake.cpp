#include "HandShake.h"
#include"KuroFunc.h"
#include"PerlinNoise.h"

void HandShake::Init()
{
	m_t = 0.0f;
}

void HandShake::Update(float arg_timeScale,const Matrix& arg_rotate)
{
	m_t += 1.0f / m_interval * arg_timeScale;

	//ŽèƒuƒŒ
	Vec3<float>handShake = { 1.0f,1.0f,0.0f };
	handShake.x = PerlinNoise::GetRand(m_t, 0.0f, m_seed.x) * m_val.x;
	handShake.y = PerlinNoise::GetRand(0.0f, m_t, m_seed.y) * m_val.y;

	m_offset = KuroMath::TransformVec3(handShake, arg_rotate);
}
