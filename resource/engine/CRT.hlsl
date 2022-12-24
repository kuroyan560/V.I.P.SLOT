#include"Math.hlsli"

struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float2 uv : TEXCOORD; //uv�l
};

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
}

cbuffer cbuff1 : register(b1)
{
    //��ʃT�C�Y
    float2 screenSize;
	//��ʘc�݋���
    float screenDistort;
    //�K�E�V�A���t�B���^�������ăT���v�����O����ۂ̏d��
    float gaussianSampleWeight;
}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.svpos = mul(parallelProjMat, pos);
    pos.z = 0.0f;
    output.uv = uv;
    return output;
}

// 2�����x�N�g�����V�[�h�l�Ƃ�������
float Rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43578.5453);
}

//3x3�̃K�E�V�A���t�B���^�������ăT���v�����O
float4 GaussianSample(float2 uv, float2 dx, float2 dy, float weight)
{
    float4 col = float4(0, 0, 0, 0);
    //����
    col += tex.Sample(smp, uv - dx - dy) * (weight / pow(2, 4)) / weight;
    //��
    col += tex.Sample(smp, uv - dx) * (weight / pow(2, 3)) / weight;
    //����
    col += tex.Sample(smp, uv - dx + dy) * (weight / pow(2, 4)) / weight;
    //��
    col += tex.Sample(smp, uv - dy) * (weight / pow(2, 3)) / weight;
    //�^��
    col += tex.Sample(smp, uv) * (weight / pow(2, 2)) / weight;
    //��
    col += tex.Sample(smp, uv + dy) * (weight / pow(2, 3)) / weight;
    //�E��
    col += tex.Sample(smp, uv + dx - dy) * (weight / pow(2, 4)) / weight;
    //�E
    col += tex.Sample(smp, uv + dx) * (weight / pow(2, 3)) / weight;
    //�E��
    col += tex.Sample(smp, uv + dx + dy) * (weight / pow(2, 4)) / weight;
    return col;
}

float CRTEase(const float x,const float base,const float offset)
{
    float tmp = frac(x + offset);
    float xx = 1 - abs(tmp * 2 - 1);
    float ease = Easing_Cubic_InOut(xx, 1.0f, 0.0f, 1.0f);
    return ease * base + base * 0.8f;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    float2 inUV = input.uv;
    
    const float floorX = frac(inUV.x * screenSize.x / 3.0f);
    const float isR = step(floorX, 0.3f);
    const float isG = step(floorX, 0.6f) * (1 - isR);
    const float isB = step(0.6f, floorX) * (1 - isR) * (1 - isG);
    
    const float2 dx = float2(1.0f / screenSize.x, 0.0f);
    const float2 dy = float2(0.0f, 1.0f / screenSize.y);
    
    float2 texUV = inUV;
    texUV += isR * -1.0f * dy;
    texUV += isG * 0.0f * dy;
    texUV += isB * 1.0f * dy;
    
    float4 col = GaussianSample(texUV, dx, dy, gaussianSampleWeight);
    
    const float floorY = frac(texUV.y * screenSize.y / 6.0f);
    const float easeR = CRTEase(floorY, col.r, Rand(texUV) * 0.1f);
    const float easeG = CRTEase(floorY, col.g, Rand(texUV) * 0.1f);
    const float easeB = CRTEase(floorY, col.b, Rand(texUV) * 0.1f);
    
    float r = isR * easeR;
    float g = isG * easeG;
    float b = isB * easeB;
    
    return float4(r, g, b, 1.0f);
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}