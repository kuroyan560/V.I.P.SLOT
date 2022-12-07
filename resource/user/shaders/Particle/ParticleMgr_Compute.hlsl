#include"ComputeParticle.hlsli"

[numthreads(THREAD_PER_NUM, 1, 1)]
void CSmain(uint3 DTid : SV_DispatchThreadID)
{
    Particle particle = g_particles[DTid.x];
    
    //���S���Ă���
    if (particle.m_alive == 0)
    {
        return;
    }
    
    //�������ԍX�V
    particle.m_lifeTimer += g_constData.m_timeScale;
    
    //�������s�����琶���t���O��؂�
    if (particle.m_initializer.m_lifeSpan <= particle.m_lifeTimer)
    {
        particle.m_alive = 0;
    }
    
    //�X�V��̃p�[�e�B�N���K�p
    g_particles[DTid.x] = particle;
}