#include"ComputeParticle.hlsli"

[numthreads(THREAD_PER_NUM, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID)
{
    Particle particle = g_particles[DTid.x];
    
    //死亡している
    if (particle.m_alive == 0)
    {
        return;
    }
    
    //生存時間更新
    particle.m_lifeTimer += g_constData.m_timeScale;
    
    //寿命が尽きたら生存フラグを切る
    if (particle.m_initializer.m_lifeSpan <= particle.m_lifeTimer)
    {
        particle.m_alive = 0;
    }
    
    //更新後のパーティクル適用
    g_particles[DTid.x] = particle;
}