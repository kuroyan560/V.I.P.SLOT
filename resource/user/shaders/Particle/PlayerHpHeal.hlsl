#include"ComputeParticle.hlsli"

[numthreads(THREAD_PER_NUM, 1, 1)]
void CSmain( uint3 DTid : SV_DispatchThreadID )
{
    Particle particle = g_particles[DTid.x];
    
    //死亡している
    if(particle.m_alive == 0)
    {
        return;
    }
    
    //寿命が半分経過していれば 1,そうでなければ 0
    float halfTime = step(particle.m_initializer.m_lifeSpan / 2.0f, particle.m_lifeTimer);
    
    //アルファ変化
    float alpha = Easing_Circ_In(
    particle.m_lifeTimer - particle.m_initializer.m_lifeSpan / 2.0f,
    particle.m_initializer.m_lifeSpan / 2.0f,
    1.0f,
    0.0f);
    particle.m_mulColor.w = halfTime * alpha + (1.0f - halfTime) * 1.0f;
    
    
    //スケール変化
    //前半
    float fistHalfScale = Easing_Back_Out(
    particle.m_lifeTimer,
    particle.m_initializer.m_lifeSpan / 2.0f,
    0.0f,
    particle.m_initializer.m_scale);
    
    //後半
    float secondHalfScale = Easing_Circ_In(
    particle.m_lifeTimer - particle.m_initializer.m_lifeSpan / 2.0f,
    particle.m_initializer.m_lifeSpan / 2.0f,
    particle.m_initializer.m_scale,
    0.0f);
    
    //時間に応じて適用するスケール変更
    particle.m_scale = secondHalfScale * halfTime + fistHalfScale * (1.0f - halfTime);
    
    //更新後のパーティクル適用
    g_particles[DTid.x] = particle;
}