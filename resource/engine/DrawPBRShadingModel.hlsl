#include"ModelInfo.hlsli"
#include"Camera.hlsli"
#include"LightInfo.hlsli"
#include"Math.hlsli"

cbuffer cbuff0 : register(b0)
{
    Camera cam;
}

cbuffer cbuff1 : register(b1)
{
    LightInfo ligNum; //�A�N�e�B�u���̃��C�g�̐��̏��
}

StructuredBuffer<DirectionLight> dirLight : register(t0);
StructuredBuffer<PointLight> pointLight : register(t1);
StructuredBuffer<SpotLight> spotLight : register(t2);
StructuredBuffer<HemiSphereLight> hemiSphereLight : register(t3);
TextureCube cubeMap : register(t4);


cbuffer cbuff2 : register(b2)
{
    matrix world;
}

cbuffer cbuff3 : register(b3)
{
    matrix bones[256]; //�{�[���s��
}

Texture2D<float4> baseTex : register(t5);
Texture2D<float4> metalnessTex : register(t6);
Texture2D<float4> normalMap : register(t7);
Texture2D<float4> roughnessTex : register(t8);
SamplerState smp : register(s0);

static float3 s_baseColor;
static float s_metalness;
static float s_roughness;

cbuffer cbuff3 : register(b4)
{
    Material material;
}

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 worldpos : POSITION;
    float3 normal : NORMAL; // �@��
    float3 tangent : TANGENT;
    float3 biNormal : BINORMAL;
    float2 uv : TEXCOORD;
    float3 reflect : REFLECT;
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
    float4 wpos = mul(world, resultPos); //���[���h�ϊ�
    output.svpos = mul(cam.view, wpos); //�r���[�ϊ�
    output.svpos = mul(cam.proj, output.svpos); //�v���W�F�N�V�����ϊ�
    output.worldpos = wpos;
    output.normal = normalize(mul(world, input.normal));
    output.tangent = normalize(mul(world, input.tangent));
    output.biNormal = normalize(mul(world, input.binormal));
    output.uv = input.uv;
    
    //�����x�N�g���Ɩ@����蔽�˃x�N�g�������߂�
    output.reflect = normalize(reflect(normalize(wpos.xyz - cam.eyePos), output.normal));
    return output;
}

struct PSOutput
{
    float4 color : SV_Target0;
    //float4 emissive : SV_Target1;
};

//Schlick�ɂ��ߎ���
float SchlickFresnel(float f0, float f90, float cosine)
{
    float m = saturate(1 - cosine);
    float m2 = m * m;
    float m5 = m2 * m2 * m;
    return lerp(f0, f90, m5);
}

//UE4��GGX���z
float DistributionGGX(float alpha,float NdotH)
{
    if (!any(alpha)) return 0.0f;
    float alpha2 = alpha * alpha;
    float t = NdotH * NdotH * (alpha2 - 1.0f) + 1.0f;
    return alpha2 / (PI * t * t);
}

float3 SchlickFresnel3(float3 f0, float3 f90, float cosine)
{
    float m = saturate(1 - cosine);
    float m2 = m * m;
    float m5 = m2 * m2 * m;
    return lerp(f0, f90, m5);
}

float3 DisneyFresnel(float LdotH)
{
    float luminance = 0.3f * s_baseColor.r + 0.6 * s_baseColor.g + 0.1f * s_baseColor.b;
    float3 tintColor = s_baseColor / luminance;
    float3 nonMetalColor = material.specular_pbr * 0.08f * tintColor;
    float3 specularColor = lerp(nonMetalColor, s_baseColor, s_metalness);
    return SchlickFresnel3(specularColor, float3(1, 1, 1), LdotH);
}

//UE4��Smith���f��
float GeometricSmith(float cosine)
{
    float k = (s_roughness + 1.0f);
    k = k * k / 8.0f;
    return cosine / (cosine * (1.0f - k) + k);
}

//���ʔ��˂̌v�Z
float3 CookTorranceSpecular(float NdotL,float NdotV,float NdotH,float LdotH)
{
    float Ds = DistributionGGX(s_roughness * s_roughness, NdotH);
    float3 Fs = DisneyFresnel(LdotH);
    float Gs = GeometricSmith(NdotL) * GeometricSmith(NdotV);
    float m = 4.0f * NdotL * NdotV;
    return Ds * Fs * Gs / m;
}

//�o�������˕��z�֐�
float3 BRDF(float3 LigDirection, float3 LigColor, float3 WorldNormal, float3 WorldPos, float3 EyePos)
{
    float3 L = -LigDirection;
    float3 V = normalize(EyePos - WorldPos);
    float NdotL = dot(WorldNormal, L);
    float NdotV = dot(WorldNormal, V);
    if (NdotL <= 0 || NdotV <= 0)
    {
        return float3(0, 0, 0);
    }
    
    float3 H = normalize(L + V);
    float NdotH = dot(WorldNormal, H);
    float LdotH = dot(L, H);
    
    float diffuseReflectance = 1.0f / PI;
    
    float energyBias = 0.5f * s_roughness;
    float Fd90 = energyBias + 2.0f * LdotH * LdotH * s_roughness;
    float FL = SchlickFresnel(1.0f, Fd90, NdotL);
    float FV = SchlickFresnel(1.0f, Fd90, NdotV);
    float energyFactor = lerp(1.0f, 1.0f / 1.51f, s_roughness);
    float Fd = FL * FV * energyFactor;
    
    float3 diffuseColor = diffuseReflectance * Fd * s_baseColor * (1 - s_metalness);
    float3 specularColor = CookTorranceSpecular(NdotL, NdotV, NdotH, LdotH) * LigColor;
    
    return (diffuseColor + specularColor);
}

PSOutput PSmain(VSOutput input) : SV_TARGET
{
    float3 normal = input.normal;
    float3 localNormal = normalMap.Sample(smp, input.uv).xyz;
    localNormal = (localNormal - 0.5f) * 2.0f; //�^���W�F���g�X�y�[�X�̖@����0�`1�͈̔͂���-1�`1�͈̔͂ɕ�������
    normal = input.tangent * localNormal.x + input.biNormal * localNormal.y + normal * localNormal.z;
    normal = normalize(normal);
    
    s_baseColor = material.baseColor + baseTex.Sample(smp, input.uv).rgb;
    s_metalness = material.metalness + metalnessTex.Sample(smp, input.uv).r;
    s_roughness = material.roughness + roughnessTex.Sample(smp, input.uv).r;
    
    float4 cubeMapLig = cubeMap.Sample(smp, input.reflect);
    
     //���C�g�̉e��
    float3 ligEffect = { 0.0f, 0.0f, 0.0f };
    ligEffect = BRDF(-input.reflect, cubeMapLig.xyz, normal, input.worldpos, cam.eyePos);
    
    //�f�B���N�V�������C�g
    for (int i = 0; i < ligNum.dirLigNum; ++i)
    {
        if (!dirLight[i].active)
            continue;
        
        float3 dir = dirLight[i].direction;
        float3 ligCol = dirLight[i].color.xyz * dirLight[i].color.w;
        ligEffect += BRDF(dir, ligCol, normal, input.worldpos, cam.eyePos);
    }
    //�|�C���g���C�g
    for (int i = 0; i < ligNum.ptLigNum; ++i)
    {
        if (!pointLight[i].active)
            continue;
        
        float3 dir = input.worldpos - pointLight[i].pos;
        dir = normalize(dir);
        float3 ligCol = pointLight[i].color.xyz * pointLight[i].color.w;
        
        //�����ɂ�錸��
        float3 distance = length(input.worldpos - pointLight[i].pos);
		//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / pointLight[i].influenceRange * distance;
		//�e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
            affect = 0.0f;
		//�e�����w���֐��I�ɂ���
        affect = pow(affect, 3.0f);
        
        ligEffect += BRDF(dir, ligCol, normal, input.worldpos, cam.eyePos) * affect;
    }
    //�X�|�b�g���C�g
    for (int i = 0; i < ligNum.spotLigNum; ++i)
    {
        if (!spotLight[i].active)
            continue;
        
        float3 ligDir = input.worldpos - spotLight[i].pos;
        ligDir = normalize(ligDir);
        float3 ligCol = spotLight[i].color.xyz * spotLight[i].color.w;
        
        //�X�|�b�g���C�g�Ƃ̋������v�Z
        float3 distance = length(input.worldpos - spotLight[i].pos);
       	//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
        float affect = 1.0f - 1.0f / spotLight[i].influenceRange * distance;
        //�e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
        if (affect < 0.0f)
            affect = 0.0f;
    //�e�����w���֐��I�ɂ���
        affect = pow(affect, 3.0f);
    
        float3 dir = normalize(spotLight[i].target - spotLight[i].pos);
        float angle = dot(ligDir, dir);
        angle = abs(acos(angle));
        affect = 1.0f - 1.0f / spotLight[i].angle * angle;
        if (affect < 0.0f)
            affect = 0.0f;
        affect = pow(affect, 0.5f);
        
        ligEffect += BRDF(dir, ligCol, normal, input.worldpos, cam.eyePos) * affect;
    }
    //�V��
    for (int i = 0; i < ligNum.hemiSphereNum; ++i)
    {
        if (!hemiSphereLight[i].active)
            continue;
        
        float t = dot(normal.xyz, hemiSphereLight[i].groundNormal);
        t = (t + 1.0f) / 2.0f;
        float3 hemiLight = lerp(hemiSphereLight[i].groundColor, hemiSphereLight[i].skyColor, t);
        ligEffect *= hemiLight;
    }
    
    float4 result = float4(ligEffect, 1.0f - material.transparent);
    
    PSOutput output;
    output.color = result;
    //output.color.xyz = cubeMapLig.xyz;
    //output.emissive = emissiveMap.Sample(smp, input.uv);
    
    ////���邳�v�Z
    //float bright = dot(result.xyz, float3(0.2125f, 0.7154f, 0.0721f));
    //if (1.0f < bright)
    //    output.emissive += result;
    //output.emissive.w = result.w;
    
    return output;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}