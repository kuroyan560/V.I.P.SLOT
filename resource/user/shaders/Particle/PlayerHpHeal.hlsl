#include"ComputeParticle.hlsli"

[numthreads(THREAD_PER_NUM, 1, 1)]
void CSmain( uint3 DTid : SV_DispatchThreadID )
{
    Particle particle = g_particles[DTid.x];
    
    //���S���Ă���
    if(particle.m_alive == 0)
    {
        return;
    }
    
    //�����������o�߂��Ă���� 1,�����łȂ���� 0
    float halfTime = step(particle.m_initializer.m_lifeSpan / 2.0f, particle.m_lifeTimer);
    
    //�A���t�@�ω�
    float alpha = Easing_Circ_In(
    particle.m_lifeTimer - particle.m_initializer.m_lifeSpan / 2.0f,
    particle.m_initializer.m_lifeSpan / 2.0f,
    1.0f,
    0.0f);
    particle.m_mulColor.w = halfTime * alpha + (1.0f - halfTime) * 1.0f;
    
    
    //�X�P�[���ω�
    //�O��
    float fistHalfScale = Easing_Back_Out(
    particle.m_lifeTimer,
    particle.m_initializer.m_lifeSpan / 2.0f,
    0.0f,
    particle.m_initializer.m_scale);
    
    //�㔼
    float secondHalfScale = Easing_Circ_In(
    particle.m_lifeTimer - particle.m_initializer.m_lifeSpan / 2.0f,
    particle.m_initializer.m_lifeSpan / 2.0f,
    particle.m_initializer.m_scale,
    0.0f);
    
    //���Ԃɉ����ēK�p����X�P�[���ύX
    particle.m_scale = secondHalfScale * halfTime + fistHalfScale * (1.0f - halfTime);
    
    //�X�V��̃p�[�e�B�N���K�p
    g_particles[DTid.x] = particle;
}