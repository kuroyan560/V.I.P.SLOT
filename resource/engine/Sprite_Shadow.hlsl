//�m�[�}���}�b�v�ŉA�e�X�v���C�g�`��
#include"LightInfo.hlsli"

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

cbuffer cbuff1 : register(b1)
{
    matrix mat; //�R�c�ϊ��s��
    float4 color; //�F(RGBA)
    float depth;  //�X�v���C�g��Z�ݒ�l
    float diffuse;  //�f�B�q���[�Y�̉e���x
    float specular; //�X�y�L�����[�̉e���x
    float lim;  //�������C�g�̉e���x
}

cbuffer cbuff2 : register(b2)
{
    float3 eyePos;
}

cbuffer cbuff3 : register(b3)
{
    LightInfo ligNum;   //�A�N�e�B�u���̃��C�g�̐��̏��
}

StructuredBuffer<DirectionLight> dirLight : register(t0);
StructuredBuffer<PointLight> pointLight : register(t1);
StructuredBuffer<SpotLight> spotLight : register(t2);
StructuredBuffer<HemiSphereLight> hemiSphereLight : register(t3);

Texture2D<float4> tex : register(t4); //�e�N�X�`��
Texture2D<float4> normalMap : register(t5); //�m�[�}���}�b�v
Texture2D<float4> emissiveMap : register(t6); //�G�~�b�V�u�}�b�v

SamplerState smp : register(s0);

//���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float2 uv : TEXCOORD; //uv�l
    float4 worldPos : WORLD_POS;
};

//���_�V�F�[�_�[
VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.svpos = mul(mat, pos);
    output.worldPos = output.svpos;
    output.svpos = mul(parallelProjMat, output.svpos);
    output.uv = uv;
    return output;
}

struct PSOutput
{
    float4 color : SV_Target0;
    float4 emissive : SV_Target1;
};

//�s�N�Z���V�F�[�_�[
PSOutput PSmain(VSOutput input)
{
    //�|�W�V����
    float3 pos = float3(input.worldPos.xy, depth);
    
    //�m�[�}���}�b�v����@�����擾
    float3 normal = normalMap.Sample(smp, input.uv).xyz;
    normal = (normal - 0.5f) * 2.0f;
    normal.y *= -1.0f;
    normal.z *= -1.0f;
    
    float3 vnormal = mul(parallelProjMat, float4(normal, 1.0f)).xyz;
    
    //���C�g�̉e��
    //float3 ligEffect = { 0.3f, 0.3f, 0.3f };
    float3 ligEffect = { 0.0f, 0.0f, 0.0f };
    
    //�f�B���N�V�������C�g
    for (int i = 0; i < ligNum.dirLigNum;++i)
    {
        float3 dir = dirLight[i].direction;
        float3 ligCol = dirLight[i].color.xyz * dirLight[i].color.w;
        ligEffect += CalcLambertDiffuse(dir, ligCol, normal) * diffuse;
        ligEffect += CalcPhongSpecular(dir, ligCol, normal, pos, eyePos) * specular;
        ligEffect += CalcLimLight(dir, ligCol, normal, vnormal) * lim;
    }
    //�|�C���g���C�g
    for (int i = 0; i < ligNum.ptLigNum;++i)
    {
        float3 dir = pos - pointLight[i].pos;
        dir = normalize(dir);
        float3 ligCol = pointLight[i].color.xyz * pointLight[i].color.w;
        
        //�����Ȃ����
        float3 diffPoint = CalcLambertDiffuse(dir, ligCol, normal);
        float3 specPoint = CalcPhongSpecular(dir, ligCol, normal, pos, eyePos);
        
        //�����ɂ�錸��
        float3 distance = length(pos - pointLight[i].pos);
		//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / pointLight[i].influenceRange * distance;
		//�e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
            affect = 0.0f;
		//�e�����w���֐��I�ɂ���
        affect = pow(affect, 3.0f);
        diffPoint *= affect;
        specPoint *= affect;
        
        ligEffect += diffPoint * diffuse;
        ligEffect += specPoint * specular;
        ligEffect += CalcLimLight(dir, ligCol, normal, vnormal) * lim;
    }
    //�X�|�b�g���C�g
    for (int i = 0; i < ligNum.spotLigNum;++i)
    {
        float3 ligDir = pos - spotLight[i].pos;
        ligDir = normalize(ligDir);
        float3 ligCol = spotLight[i].color.xyz * spotLight[i].color.w;
        
        //�����Ȃ����
        float3 diffSpotLight = CalcLambertDiffuse(ligDir, ligCol, normal);
        float3 specSpotLight = CalcPhongSpecular(ligDir, ligCol, normal, pos, eyePos);
        
        //�X�|�b�g���C�g�Ƃ̋������v�Z
        float3 distance = length(pos - spotLight[i].pos);
       	//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / spotLight[i].influenceRange * distance;
        //�e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
            affect = 0.0f;
    //�e�����w���֐��I�ɂ���
        affect = pow(affect, 3.0f);
        diffSpotLight *= affect;
        specSpotLight *= affect;
    
        float3 spotlim = CalcLimLight(ligDir, ligCol, normal, vnormal) * affect;
        
        float3 dir = normalize(spotLight[i].target - spotLight[i].pos);
        float angle = dot(ligDir, dir);
        angle = abs(acos(angle));
        affect = 1.0f - 1.0f / spotLight[i].angle * angle;
        if (affect < 0.0f)
            affect = 0.0f;
        affect = pow(affect, 0.5f);
        
        ligEffect += diffSpotLight * affect * diffuse;
        ligEffect += specSpotLight * affect * specular;
        ligEffect += spotlim * affect * lim;
    }
    //�V��
    for (int i = 0; i < ligNum.hemiSphereNum;++i)
    {
        float t = dot(normal.xyz, hemiSphereLight[i].groundNormal);
        t = (t + 1.0f) / 2.0f;
        float3 hemiLight = lerp(hemiSphereLight[i].groundColor, hemiSphereLight[i].skyColor, t);
        ligEffect += hemiLight;
    }
    
    float4 result = tex.Sample(smp, input.uv);
    result.xyz *= ligEffect;
    
    PSOutput output;
    output.color = result * color;
    
    output.emissive = emissiveMap.Sample(smp, input.uv);
    
    //���邳�v�Z
    float bright = dot(result.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    if (1.0f < bright)
        output.emissive += result;
    output.emissive.w = result.w;
    output.emissive *= color;
    
    return output;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}