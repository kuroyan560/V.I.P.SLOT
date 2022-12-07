struct ParticleInitializer
{
    float4 m_mulColor : EMIT_MUL_COLOR;
    float2 m_pos : EMIT_POSITION;
    float2 m_vec : EMIT_VEC;
    float m_speed : EMIT_SPEED;
    float m_scale : EMIT_SCALE;
    float m_radian : EMIT_RADIAN;
    float m_lifeSpan : LIFE_SPAN;
    uint m_texIdx : EMIT_TEX_IDX;
};

struct Particle
{
    ParticleInitializer m_initializer;
    float4 m_mulColor : MUL_COLOR;
    float2 m_pos : POSITION;
    float m_speed : SPEED;
    float m_scale : SCALE;
    float m_radian : RADIAN;
    float m_lifeTimer : LIFE_TIMER;
    uint m_alive : ALIVE;
    uint m_texIdx : TEX_IDX;
};