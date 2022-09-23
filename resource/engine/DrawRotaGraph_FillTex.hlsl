cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

struct VSOutput
{
    float4 center : CENTER;
    float2 extRate : EXT_RATE;
    float radian : RADIAN;
    float2 rotaCenterUV : ROTA_CENTER_UV;
    float srcAlpha : SRC_ALPHA;
    int2 miror : MIROR;
    float2 leftUpPaintUV : PAINT_UV_L_U;
    float2 rightBottomPaintUV : PAINT_UV_R_B;
};

VSOutput VSmain(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float2 leftUpPaintUV : PAINT_UV_L_U;
    float2 rightBottomPaintUV : PAINT_UV_R_B;
    float srcAlpha : SRC_ALPHA;
};

Texture2D<float4> destTex : register(t0);
Texture2D<float4> srcTex : register(t1);
SamplerState smp : register(s0);

float2 RotateFloat2(float2 Pos,float Radian)
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
    uint2 texSize;
    destTex.GetDimensions(texSize.x, texSize.y);
    
    float width_h = texSize.x * input[0].extRate.x / 2.0f;
    float height_h = texSize.y * input[0].extRate.y / 2.0f;
    
    float2 rotateCenter = input[0].center.xy;
    rotateCenter += texSize.xy * input[0].extRate * (input[0].rotaCenterUV - float2(0.5f, 0.5f));
    
    GSOutput element;
    element.leftUpPaintUV = input[0].leftUpPaintUV;
    element.rightBottomPaintUV = input[0].rightBottomPaintUV;
    element.srcAlpha = input[0].srcAlpha;
        
    //����
    element.pos = input[0].center;
    element.pos.x -= width_h;
    element.pos.y += height_h;
    element.pos.xy = rotateCenter + RotateFloat2(element.pos.xy - rotateCenter, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].miror.x, 1.0f - input[0].miror.y);
    output.Append(element);
    
    //����
    element.pos = input[0].center;
    element.pos.x -= width_h;
    element.pos.y -= height_h;
    element.pos.xy = rotateCenter + RotateFloat2(element.pos.xy - rotateCenter, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].miror.x, 0.0f + input[0].miror.y);
    output.Append(element);
    
     //�E��
    element.pos = input[0].center;
    element.pos.x += width_h;
    element.pos.y += height_h;
    element.pos.xy = rotateCenter + RotateFloat2(element.pos.xy - rotateCenter, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].miror.x, 1.0f - input[0].miror.y);
    output.Append(element);
    
    //�E��
    element.pos = input[0].center;
    element.pos.x += width_h;
    element.pos.y -= height_h;
    element.pos.xy = rotateCenter + RotateFloat2(element.pos.xy - rotateCenter, input[0].radian);
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].miror.x, 0.0f + input[0].miror.y);
    output.Append(element);
}

float4 PSmain(GSOutput input) : SV_TARGET
{
    float4 texCol = destTex.Sample(smp, input.uv);
    
    if (input.uv.x < input.leftUpPaintUV.x)
    {
        return texCol;
    }
    if (input.uv.y < input.leftUpPaintUV.y)
    {
        return texCol;
    }
    if (input.rightBottomPaintUV.x < input.uv.x)
    {
        return texCol;
    }
    if (input.rightBottomPaintUV.y < input.uv.y)
    {
        return texCol;
    }
    
    float4 srcCol = srcTex.Sample(smp, input.uv);
    texCol.xyz = texCol.xyz * (1.0f - srcCol.w * input.srcAlpha) + srcCol.xyz * srcCol.w * input.srcAlpha;
    return texCol;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}