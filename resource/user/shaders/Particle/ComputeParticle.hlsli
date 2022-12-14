#include"Particle.hlsli"
#include"../../../engine/Math.hlsli"

//ParticleMgr.h側のスレッド数と合わせる
static const int THREAD_PER_NUM = 10;

struct CommonConstData
{
    float m_timeScale;
};

cbuffer cbuff0 : register(b0)
{
    CommonConstData g_constData;
}
RWStructuredBuffer<Particle> g_particles : register(u0);
