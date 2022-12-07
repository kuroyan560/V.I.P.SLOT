#include"Particle.hlsli"


cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //平行投影行列
};

Texture2D<float4> tex0 : register(t0);
Texture2D<float4> tex1 : register(t1);
Texture2D<float4> tex2 : register(t2);
Texture2D<float4> tex3 : register(t3);
Texture2D<float4> tex4 : register(t4);
Texture2D<float4> tex5 : register(t5);
Texture2D<float4> tex6 : register(t6);
Texture2D<float4> tex7 : register(t7);
Texture2D<float4> tex8 : register(t8);
Texture2D<float4> tex9 : register(t9);
SamplerState smp : register(s0);

struct VSOutput
{
    float4 m_mulColor : MUL_COLOR;
    float2 m_pos : POSITION;
    float m_radian : RADIAN;
    min16int m_alive : ALIVE;
    float m_scale : DRAW_SCALE;
    uint m_texIdx : TEX_IDX;
};

VSOutput VSmain(Particle input)
{
    VSOutput output;
    output.m_mulColor = input.m_mulColor;
    output.m_pos = input.m_pos;
    output.m_radian = input.m_radian;
    output.m_alive = input.m_alive;
    output.m_scale = input.m_scale / 2.0f;
    output.m_texIdx = input.m_texIdx;
    return output;
}

struct GSOutput
{
    float4 m_mulColor : MUL_COLOR;
    float4 m_pos : SV_POSITION;
    float2 m_uv : TEXCOORD;
    uint m_texIdx : TEX_IDX;
};

float2 RotateFloat2(float2 Pos, float Radian)
{
    float2 result;
    result.x = Pos.x * cos(Radian) - Pos.y * sin(Radian);
    result.y = Pos.y * cos(Radian) + Pos.x * sin(Radian);
    return result;
}

[maxvertexcount(4)]
void GSmain(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    if (!input[0].m_alive)
        return;
    
    GSOutput element;
    element.m_mulColor = input[0].m_mulColor;
    element.m_texIdx = input[0].m_texIdx;
    
    float4 pos = float4(input[0].m_pos, 0.0f, 1.0f);
        
    //左下
    element.m_pos = pos;
    element.m_pos.x -= input[0].m_scale;
    element.m_pos.y += input[0].m_scale;
    element.m_pos.xy = input[0].m_pos.xy + RotateFloat2(element.m_pos.xy - input[0].m_pos.xy, input[0].m_radian);
    element.m_pos = mul(parallelProjMat, element.m_pos);
    element.m_uv = float2(0.0f, 1.0f);
    output.Append(element);
    
    //左上
    element.m_pos = pos;
    element.m_pos.x -= input[0].m_scale;
    element.m_pos.y -= input[0].m_scale;
    element.m_pos.xy = input[0].m_pos.xy + RotateFloat2(element.m_pos.xy - input[0].m_pos.xy, input[0].m_radian);
    element.m_pos = mul(parallelProjMat, element.m_pos);
    element.m_uv = float2(0.0f, 0.0f);
    output.Append(element);
    
     //右下
    element.m_pos = pos;
    element.m_pos.x += input[0].m_scale;
    element.m_pos.y += input[0].m_scale;
    element.m_pos.xy = input[0].m_pos.xy + RotateFloat2(element.m_pos.xy - input[0].m_pos.xy, input[0].m_radian);
    element.m_pos = mul(parallelProjMat, element.m_pos);
    element.m_uv = float2(1.0f, 1.0f);
    output.Append(element);
    
    //右上
    element.m_pos = pos;
    element.m_pos.x += input[0].m_scale;
    element.m_pos.y -= input[0].m_scale;
    element.m_pos.xy = input[0].m_pos.xy + RotateFloat2(element.m_pos.xy - input[0].m_pos.xy, input[0].m_radian);
    element.m_pos = mul(parallelProjMat, element.m_pos);
    element.m_uv = float2(1.0f, 0.0f);
    output.Append(element);
}


struct PSOutput
{
    float4 color : SV_Target0;
};

PSOutput PSmain(GSOutput input) : SV_TARGET
{
    //ポジション
    float4 result;
    if (input.m_texIdx == 0)
        result = tex0.Sample(smp, input.m_uv);
    if (input.m_texIdx == 1)
        result = tex1.Sample(smp, input.m_uv);
    if (input.m_texIdx == 2)
        result = tex2.Sample(smp, input.m_uv);
    if (input.m_texIdx == 3)
        result = tex3.Sample(smp, input.m_uv);
    if (input.m_texIdx == 4)
        result = tex4.Sample(smp, input.m_uv);
    if (input.m_texIdx == 5)
        result = tex5.Sample(smp, input.m_uv);
    if (input.m_texIdx == 6)
        result = tex6.Sample(smp, input.m_uv);
    if (input.m_texIdx == 7)
        result = tex7.Sample(smp, input.m_uv);
    if (input.m_texIdx == 8)
        result = tex8.Sample(smp, input.m_uv);
    if (input.m_texIdx == 9)
        result = tex9.Sample(smp, input.m_uv);
    
    result *= input.m_mulColor;
    
    PSOutput output;
    output.color = result;
    
    return output;
}