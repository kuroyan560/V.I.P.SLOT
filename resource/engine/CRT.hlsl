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
    float screenSizeX;
    float screenSizeY;
	//��ʘc�݋���
    float screenDistort;
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

//UV����ʂ��c�Ȃ��Ă���悤�ɘc�܂���
float2 Distort(float2 uv, float rate)
{
    uv /= 1 - length(uv) * rate;
    return uv;
}

float2 NoiseSamplingUV(float2 inUV,float2 screenSize,float2 texUV)
{
    const float floorX = frac(inUV.x * screenSize.x / 3.0f);
    const float isR = step(0.3f, floorX);
    const float isG = step(0.6f, floorX) * (1 - isR);
    const float isB = step(floorX, 0.6f) * (1 - isG);
    
    const float dx = float2(1.0f / screenSize.x, 0.0f);
    const float dy = float2(0.0f, 1.0f / screenSize.y);
    
    texUV += isR * -1.0f * dy;
    texUV += isG * 0.0f * dy;
    texUV += isB * 1.0f * dy;
    return texUV;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    float2 inUV = input.uv;
    
    //��ʂ�c�܂����ۂ�UV ( -0.5f ~ 0.5f )
    float2 distortUV = Distort(input.uv - 0.5f, screenDistort);
    
    //��ʊO�Ȃ獕
    if (max(abs(distortUV.x) - 0.5f, abs(distortUV.y) - 0.5f) > 0)
    {
        return float4(0, 0, 0, 1);
    }
    
    //�e�N�X�`���̃T���v�����O������UV ( 0.0f ~ 1.0f )
    float2 texUV = distortUV + 0.5f;
    
    texUV = NoiseSamplingUV(inUV, float2(screenSizeX, screenSizeY), texUV);
    
    return tex.Sample(smp, texUV);
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}