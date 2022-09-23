#include"ModelInfo.hlsli"
#include"Camera.hlsli"

cbuffer cbuff0 : register(b0)
{
    Camera cam;
}

cbuffer cbuff1 : register(b1)
{
    matrix world;
}

cbuffer cbuff2 : register(b2)
{
    Camera lightCam;
}

Texture2D<float4> tex : register(t0);
Texture2D<float4> shadowMap : register(t1);
SamplerState smp : register(s0);
SamplerComparisonState shadowMapSmp : register(s1);

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 posInLVP : TEXCOORD1;    //���C�g�r���[�X�N���[����Ԃł̃s�N�Z���̍��W
};

VSOutput VSmain(Vertex input)
{
    float4 resultPos = input.pos;
	
	////�{�[���s��
	////BDEF4(�{�[��4����3�ƁA���ꂼ��̃E�F�C�g�l�B�E�F�C�g���v��1.0�ł���ۏ�͂��Ȃ�)
 //   if (input.boneNo[2] != NO_BONE)
 //   {
 //       int num;
        
 //       if (input.boneNo[3] != NO_BONE)    //�{�[���S��
 //       {
 //           num = 4;
 //       }
 //       else //�{�[���R��
 //       {
 //           num = 3;
 //       }
        
 //       matrix mat = bones[input.boneNo[0]] * input.weight[0];
 //       for (int i = 1; i < num; ++i)
 //       {
 //           mat += bones[input.boneNo[i]] * input.weight[i];
 //       }
 //       resultPos = mul(mat, input.pos);
 //   }
	////BDEF2(�{�[��2�ƁA�{�[��1�̃E�F�C�g�l(PMD����))
 //   else if (input.boneNo[1] != NONE)
 //   {
 //       matrix mat = bones[input.boneNo[0]] * input.weight[0];
 //       mat += bones[input.boneNo[1]] * (1 - input.weight[0]);
        
 //       resultPos = mul(mat, input.pos);
 //   }
	////BDEF1(�{�[���̂�)
 //   else if (input.boneNo[0] != NONE)
 //   {
 //       resultPos = mul(bones[input.boneNo[0]], input.pos);
 //   }
	
    VSOutput output;
    float4 worldPos = mul(world, resultPos);
    
    //�ʏ�̍��W�ϊ�
    output.svpos = mul(cam.proj, mul(cam.view, worldPos));
    output.uv = input.uv;
    
    //���C�g�r���[�X�N���[����Ԃ̍��W
    output.posInLVP = mul(lightCam.proj, mul(lightCam.view, worldPos));
    //���C�g���猩���[�x�l���v�Z����
    output.posInLVP.z = length(worldPos.xyz - lightCam.eyePos) / 1000.0f;
    
    return output;
}


float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 color = tex.Sample(smp, input.uv);
    
    //���C�g�r���[�X�N���[����Ԃ���UV��Ԃɍ��W�ϊ�
    float2 shadowMapUV = input.posInLVP.xy / input.posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;
    
    //���C�g�r���[�X�N���[����Ԃł�Z�l���v�Z����
    float zInLVP = input.posInLVP.z;

    if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
        && shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f)
    {
        // �v�Z����UV���W���g���āA�V���h�E�}�b�v����[�x�l���T���v�����O
        float2 shadowValue = shadowMap.Sample(smp, shadowMapUV).xy;
        
        //���̃s�N�Z�����B������Ă��邩
        if (zInLVP > shadowValue.r && zInLVP <= 1.0f)
        {
            // �Օ�����Ă���Ȃ�A�`�F�r�V�F�t�̕s�����𗘗p���Č���������m�������߂�
            float depth_sq = shadowValue.x * shadowValue.x;
            
             // ���̃O���[�v�̕��U������߂�
            // ���U���傫���قǁAvariance�̐��l�͑傫���Ȃ�
            float variance = min(max(shadowValue.y - depth_sq, 0.0001f), 1.0f);

            // ���̃s�N�Z���̃��C�g���猩���[�x�l�ƃV���h�E�}�b�v�̕��ς̐[�x�l�̍������߂�
            float md = zInLVP - shadowValue.x;

            // �����͂��m�������߂�
            float lit_factor = variance / (variance + md * md);
            
             // �V���h�E�J���[�����߂�
            float3 shadowColor = color.xyz * 0.5f;
            
            // ����������m�����g���Ēʏ�J���[�ƃV���h�E�J���[����`�⊮
            color.xyz = lerp(shadowColor, color.xyz, lit_factor);
        }
    }
    
    return color;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}