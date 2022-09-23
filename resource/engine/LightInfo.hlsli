//�f�B���N�V�������C�g
struct DirectionLight
{
    uint active;
    float4 color;
    float3 direction;
};

//�|�C���g���C�g
struct PointLight
{
    uint active;
    float4 color;
    float3 pos;
    float influenceRange;
};

//�X�|�b�g���C�g
struct SpotLight
{
    uint active;
    float4 color;
    float3 pos;
    float influenceRange;
    float3 target;
    float angle;
};

//�V��
struct HemiSphereLight
{
    uint active;
    float4 skyColor;
    float4 groundColor;
    float3 groundNormal;
};

//�A�N�e�B�u���̃��C�g�̐�
struct LightInfo
{
    int dirLigNum;
    int ptLigNum;
    int spotLigNum;
    int hemiSphereNum;
};

float3 CalcLambertDiffuse(float3 DirDirection, float3 DirColor, float3 Normal)
{
    float t = saturate(dot(normalize(Normal), -DirDirection));
	//�g�U���ˌ�
    return DirColor * t;
}

//���K��Lambert
float3 CalcNormalizeLambertDiffuse(float3 DirDirection, float3 DirColor, float3 Normal)
{
    float t = dot(Normal, DirDirection);
    t *= -1.0f;
    if (t < 0.0f)
        t = 0.0f;
	
	//�g�U���ˌ�
    return DirColor * t / 3.1415926f;
}

float3 CalcPhongSpecular(float3 DirDirection, float3 DirColor, float3 WorldNormal, float3 WorldPos, float3 EyePos)
{
	//���˃x�N�g��
    //float3 refVec = reflect(DirDirection, WorldNormal);

	//���ʔ��˂̋���
    float3 toEye = normalize(EyePos - WorldPos);
    //float t = dot(refVec, toEye);
    float t = saturate(dot(normalize(WorldNormal), normalize(-DirDirection + toEye)));
    
    //if (t < 0.0f)
    //{
        //t = 0.0f;
    //}

	//���ʔ��˂̋������i��
    //t = pow(t, 5.0f);
    t = pow(t, 5.0f);

	//���ʔ��ˌ�
    return DirColor * t;
}

float3 CalcLimLight(float3 Dir, float3 Color, float3 WorldNormal, float3 ViewNormal)
{
    float power1 = 1.0f - max(0.0f, dot(Dir, WorldNormal));
    float power2 = 1.0f - max(0.0f, ViewNormal.z * -1.0f);
    float limPower = power1 * power2;
    limPower = pow(limPower, 1.3f);
    return limPower * Color;
}

float3 CalcLigDirection(DirectionLight DirLight, float3 WorldNormal, float3 WorldPos, float3 ViewNormal, float3 EyePos)
{
    return CalcLambertDiffuse(DirLight.direction, DirLight.color.xyz, WorldNormal)
		+ CalcPhongSpecular(DirLight.direction, DirLight.color.xyz, WorldNormal, WorldPos, EyePos)
        + CalcLimLight(DirLight.direction, DirLight.color.xyz, WorldNormal, ViewNormal);
}

float3 CalcLigPoint(PointLight PtLight, float3 WorldPos, float3 WorldNormal, float3 ViewNormal, float3 EyePos)
{
    float3 ligDir = WorldPos - PtLight.pos;
    ligDir = normalize(ligDir);
		//�����Ȃ�
    float3 diffPoint = CalcLambertDiffuse(ligDir, PtLight.color.xyz, WorldNormal);
    float3 specPoint = CalcPhongSpecular(ligDir, PtLight.color.xyz, WorldNormal, WorldPos, EyePos);
	//�����ɂ�錸��
    float3 distance = length(WorldPos - PtLight.pos);
		//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
    float affect = 1.0f - 1.0f / PtLight.influenceRange * distance;
		//�e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
    if (affect < 0.0f)
        affect = 0.0f;
		//�e�����w���֐��I�ɂ���
    affect = pow(affect, 3.0f);
    diffPoint *= affect;
    specPoint *= affect;
    
    return diffPoint + specPoint + CalcLimLight(ligDir, PtLight.color.xyz, WorldNormal, ViewNormal) * affect;
}

float3 CalcLigSpot(SpotLight SpotLight, float3 WorldPos, float3 WorldNormal, float3 ViewNormal, float3 EyePos)
{
    float3 ligDir = WorldPos - SpotLight.pos;
    ligDir = normalize(ligDir);
    float3 diffSpotLight = CalcLambertDiffuse(ligDir, SpotLight.color.xyz, WorldNormal);
    float3 specSpotLight = CalcPhongSpecular(ligDir, SpotLight.color.xyz, WorldNormal, WorldPos, EyePos);
        //�X�|�b�g���C�g�Ƃ̋������v�Z
    float3 distance = length(WorldPos - SpotLight.pos);
       	//�e�����͋����ɔ�Ⴕ�ď������Ȃ��Ă���
    float affect = 1.0f - 1.0f / SpotLight.influenceRange * distance;
        //�e���͂��}�C�i�X�ɂȂ�Ȃ��悤�ɕ␳��������
    if (affect < 0.0f)
        affect = 0.0f;
    //�e�����w���֐��I�ɂ���
    affect = pow(affect, 3.0f);
    diffSpotLight *= affect;
    specSpotLight *= affect;
    
    float3 lim = CalcLimLight(ligDir, SpotLight.color.xyz, WorldNormal, ViewNormal) * affect;
        
    float3 dir = normalize(SpotLight.target - SpotLight.pos);
    float angle = dot(ligDir, dir);
    angle = abs(acos(angle));
    affect = 1.0f - 1.0f / SpotLight.angle * angle;
    if (affect < 0.0f)
        affect = 0.0f;
    affect = pow(affect, 0.5f);
    diffSpotLight *= affect;
    specSpotLight *= affect;
    lim *= affect;
    return diffSpotLight + specSpotLight + lim;
}