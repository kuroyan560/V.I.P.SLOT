cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

struct VSOutput
{
    float4 leftUpPos : POSITION_L_U;
    float4 rightBottomPos : POSITION_R_B;
    float4 paintColor : PAINT_COLOR;
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
    float4 paintColor : PAINT_COLOR;
    float2 leftUpPaintUV : PAINT_UV_L_U;
    float2 rightBottomPaintUV : PAINT_UV_R_B;
};


[maxvertexcount(4)]
void GSmain(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    float width = input[0].rightBottomPos.x - input[0].leftUpPos.x;
    
    GSOutput element;
    element.paintColor = input[0].paintColor;
    element.leftUpPaintUV = input[0].leftUpPaintUV;
    element.rightBottomPaintUV = input[0].rightBottomPaintUV;
        
    //����
    element.pos = input[0].rightBottomPos;
    element.pos.x -= width;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].miror.x, 1.0f - input[0].miror.y);
    output.Append(element);
    
    //����
    element.pos = input[0].leftUpPos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(0.0f + input[0].miror.x, 0.0f + input[0].miror.y);
    output.Append(element);
    
     //�E��
    element.pos = input[0].rightBottomPos;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].miror.x, 1.0f - input[0].miror.y);
    output.Append(element);
    
    //�E��
    element.pos = input[0].leftUpPos;
    element.pos.x += width;
    element.pos = mul(parallelProjMat, element.pos);
    element.uv = float2(1.0f - input[0].miror.x, 0.0f + input[0].miror.y);
    output.Append(element);
}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain_None(GSOutput input) : SV_TARGET
{
    float4 texCol = tex.Sample(smp, input.uv);
    
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
    
    return float4(input.paintColor.xyz, texCol.w * input.paintColor.w);
}

float4 PSmain_Mul(GSOutput input) : SV_TARGET
{
    float4 texCol = tex.Sample(smp, input.uv);
    
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
    
    return input.paintColor * texCol;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}