#include"../../engine/Camera.hlsli"

cbuffer cbuff0 : register(b0)
{
    Camera cam;
};

struct Vertex
{
    int isAlive : ALIVE;
    float4 center : POSITION;
    float scale : SCALE;
    float rotate : ROTATE;
    float alpha : ALPHA;
    float lifeTimer : LIFE_TIMER;
    int lifeSpan : LIFE_SPAN;
    float blur : BLUR;
    float uvRadiusOffset : UV_RADIUS_OFFSET;
    float circleThickness : CIRCLE_THICKNESS;
    float circleRadius : CIRCLE_RADIUS;
};

Vertex VSmain(Vertex input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float alpha : ALPHA;
    float blur : BLUR;
    float uvRadiusOffset : UV_RADIUS_OFFSET;
    float circleThickness : CIRCLE_THICKNESS;
    float circleRadius : CIRCLE_RADIUS;
};

Texture2D<float4> displacementNoiseTex : register(t0);
Texture2D<float4> alphaNoiseTex : register(t1);
SamplerState smp : register(s0);

float2 RotateFloat2(float2 Pos, float Radian)
{
    float2 result;
    result.x = Pos.x * cos(Radian) - Pos.y * sin(Radian);
    result.y = Pos.y * cos(Radian) + Pos.x * sin(Radian);
    return result;
}

static const int2 SIZE = int2(15, 15);

[maxvertexcount(4)]
void GSmain(
	point Vertex input[1],
	inout TriangleStream<GSOutput> output
)
{
    if (!input[0].isAlive)
        return;
    
    float width_h = (SIZE.x * input[0].scale) / 2.0f;
    float height_h = (SIZE.y * input[0].scale) / 2.0f;
    
    float2 rotateCenter = input[0].center.xy;
    
    GSOutput element;
    element.alpha = input[0].alpha;
    element.blur = input[0].blur;
    element.uvRadiusOffset = input[0].uvRadiusOffset;
    element.circleThickness = input[0].circleThickness;
    element.circleRadius = input[0].circleRadius;
    
    float4 offset = float4(0, 0, 0, 0);
    
    //����
    element.pos = input[0].center;
    //offset = mul(cam.billBoardY, float4(-width_h, -height_h, 0, 0));
    offset = mul(cam.billBoardY, float4(RotateFloat2(float2(-width_h, -height_h), input[0].rotate), 0, 0));
    element.pos = mul(cam.proj, mul(cam.view, element.pos + offset));
    element.uv = float2(0.0f, 0.0f);
    output.Append(element);
    
    //����
    element.pos = input[0].center;
    //offset = mul(cam.billBoardY, float4(-width_h, height_h, 0, 0));
    offset = mul(cam.billBoardY, float4(RotateFloat2(float2(-width_h, height_h), input[0].rotate), 0, 0));
    //element.pos.xy = rotateCenter + RotateFloat2(element.pos.xy - rotateCenter, input[0].rotate);
    element.pos = mul(cam.proj, mul(cam.view, element.pos + offset));
    element.uv = float2(0.0f, 1.0f);
    output.Append(element);
    
     //�E��
    element.pos = input[0].center;
    //offset = mul(cam.billBoardY, float4(width_h, -height_h, 0, 0));
    offset = mul(cam.billBoardY, float4(RotateFloat2(float2(width_h, -height_h), input[0].rotate), 0, 0));
    element.pos = mul(cam.proj, mul(cam.view, element.pos + offset));
    element.uv = float2(1.0f, 0.0f);
    output.Append(element);
    
     //�E��
    element.pos = input[0].center;
    //offset = mul(cam.billBoardY, float4(width_h, height_h, 0, 0));
    offset = mul(cam.billBoardY, float4(RotateFloat2(float2(width_h, height_h), input[0].rotate), 0, 0));
    element.pos = mul(cam.proj, mul(cam.view, element.pos + offset));
    element.uv = float2(1.0f, 1.0f);
    output.Append(element);
}

//�ꎞ�i�[�p
static float _circleThicknes;
static float _circleRadius;

float4 GetCirclePixel(float2 uv)
{
    float dist = length(uv - float2(0.5f, 0.5f));
    float differ = abs(dist - _circleRadius);
    return float4(1, 1, 1, 1) * step(differ, _circleThicknes / 2.0f);
}

float4 GetPixelColor(float2 uv)
{
    float displacementNoise = displacementNoiseTex.Sample(smp, uv).r;
    
    //��������O���Ɍ�������
    float2 toOutVec = uv - float2(0.5f, 0.5f);
    
    //�����_���ɂ����
    uv += normalize(toOutVec) * displacementNoise;
    
    //�ʏ�̃e�N�X�`��
    float4 result = GetCirclePixel(uv);
    
    //�A���t�@�m�C�Y
    float alphaNoise = alphaNoiseTex.Sample(smp, uv).r;
    alphaNoise *= 9.0f; //�R���g���X�g���グ��
    result.w *= alphaNoise;
    return result;
}

static const float2 VIEW_PORT_OFFSET = (float2(0.5f, 0.5f) / float2(1280.0f, 720.0f));
float4 PSmain(GSOutput input) : SV_TARGET
{
    float2 toOutVec = input.uv - float2(0.5f, 0.5f);    //���S����O���֌�����UV�x�N�g��
    float len = length(toOutVec);
    toOutVec = normalize(toOutVec);
    input.uv += -toOutVec * input.uvRadiusOffset;
    
    _circleRadius = input.circleRadius + 1.0f * input.uvRadiusOffset;   //�O���ɂ����قǔ��a���傫���i���S����摜���[�v���ɂ������邽�߁j
    _circleThicknes = input.circleThickness;
    
    //�u���[�̂��߂ɂ������̃s�N�Z�����T���v�����O
    float2 offset = toOutVec * VIEW_PORT_OFFSET;
    offset *= (len * input.blur);
    float4 result = GetPixelColor(input.uv) * 0.19f;
    result += GetPixelColor(input.uv + offset         ) * 0.17f;
    result += GetPixelColor(input.uv + offset * 2.0f) * 0.15f;
    result += GetPixelColor(input.uv + offset * 3.0f) * 0.13f;
    result += GetPixelColor(input.uv + offset * 4.0f) * 0.11f;
    result += GetPixelColor(input.uv + offset * 5.0f) * 0.09f;
    result += GetPixelColor(input.uv + offset * 6.0f) * 0.07f;
    result += GetPixelColor(input.uv + offset * 7.0f) * 0.05f;
    result += GetPixelColor(input.uv + offset * 8.0f) * 0.04f;
    result += GetPixelColor(input.uv + offset * 9.0f) * 0.01f;
    
    //���邭����
    result *= 2.0f;
    
    //�F����ۂ��i�A���t�@�l�������Ɛ����j
    result.xyz = lerp(float3(0.33f, 0.1f, 0.73f), float3(0.65f, 0.64f, 0.94f), result.w);
    result.w *= input.alpha;
    result.xyz *= clamp(result.w, 0.0f, 1.0f);
    return result;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}