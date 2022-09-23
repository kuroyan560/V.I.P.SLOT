#include"ModelInfo.hlsli"
#include"Camera.hlsli"

cbuffer cbuff0 : register(b0)
{
    Camera lightCamera;
}

cbuffer cbuff1 : register(b1)
{
    matrix world;
}

cbuffer cbuff2 : register(b2)
{
    matrix bones[256]; //�{�[���s��
}

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float2 depth : TEXCOORD1;
};

VSOutput VSmain(Vertex input)
{
    float4 resultPos = input.pos;
	
	//�{�[���s��
	//BDEF4(�{�[��4����3�ƁA���ꂼ��̃E�F�C�g�l�B�E�F�C�g���v��1.0�ł���ۏ�͂��Ȃ�)
    if (input.boneNo[2] != NO_BONE)
    {
        int num;
        
        if (input.boneNo[3] != NO_BONE)    //�{�[���S��
        {
            num = 4;
        }
        else //�{�[���R��
        {
            num = 3;
        }
        
        matrix mat = bones[input.boneNo[0]] * input.weight[0];
        for (int i = 1; i < num; ++i)
        {
            mat += bones[input.boneNo[i]] * input.weight[i];
        }
        resultPos = mul(mat, input.pos);
    }
	//BDEF2(�{�[��2�ƁA�{�[��1�̃E�F�C�g�l(PMD����))
    else if (input.boneNo[1] != NO_BONE)
    {
        matrix mat = bones[input.boneNo[0]] * input.weight[0];
        mat += bones[input.boneNo[1]] * (1 - input.weight[0]);
        
        resultPos = mul(mat, input.pos);
    }
	//BDEF1(�{�[���̂�)
    else if (input.boneNo[0] != NO_BONE)
    {
        resultPos = mul(bones[input.boneNo[0]], input.pos);
    }
	
    VSOutput output;
    float4 worldPos = mul(world, resultPos);
    output.svpos = mul(lightCamera.proj, mul(lightCamera.view, worldPos));
    
    //���C�g���猩���[�x�l�i�[�x�l�ő�1000) �Ƃ��̂Q��
    output.depth.x = length(worldPos.xyz - lightCamera.eyePos) / 1000.0f;
    output.depth.y = output.depth.x * output.depth.x;
    return output;
}


float4 PSmain(VSOutput input) : SV_TARGET
{
    return float4(input.depth.x, input.depth.y, 0.0f, 1.0f);
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}