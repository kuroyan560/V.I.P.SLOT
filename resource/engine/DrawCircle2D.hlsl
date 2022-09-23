cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

struct VSOutput
{
    float4 center : CENTER;
    float radius : RADIUS;
    float4 color : COLOR;
    uint fillFlg : FILL;
    int thickness : THICKNESS;
};

VSOutput VSmain(VSOutput input)
{
    return input;
}

struct GSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

static const int VERT_NUM = 64;
static const float PI = 3.14159265f;
static const float RADIAN_UINT = PI * 2 / (VERT_NUM - 1);

[maxvertexcount(VERT_NUM * 2)]
void GSmain(
	point VSOutput input[1],
	inout TriangleStream<GSOutput> output
)
{
    GSOutput element;
    element.color = input[0].color;
    
    for (int i = 0; i < VERT_NUM - 1;++i)
    {
        float rad = RADIAN_UINT * i;
        
        //����
        float innerRadius = (input[0].radius - input[0].thickness) * input[0].fillFlg;
        element.pos = input[0].center;
        element.pos.x += cos(rad) * innerRadius;
        element.pos.y += sin(rad) * innerRadius;
        element.pos = mul(parallelProjMat, element.pos);
        output.Append(element);
        
        //�O��
        element.pos = input[0].center;
        element.pos.x += cos(rad) * input[0].radius;
        element.pos.y += sin(rad) * input[0].radius;
        element.pos = mul(parallelProjMat, element.pos);
        output.Append(element);
    }
    
    float rad = 0.0f;
    
    //����
    float innerRadius = (input[0].radius - input[0].thickness) * input[0].fillFlg;
    element.pos = input[0].center;
    element.pos.x += cos(rad) * innerRadius;
    element.pos.y += sin(rad) * innerRadius;
    element.pos = mul(parallelProjMat, element.pos);
    output.Append(element);
        
    //�O��
    element.pos = input[0].center;
    element.pos.x += cos(rad) * input[0].radius;
    element.pos.y += sin(rad) * input[0].radius;
    element.pos = mul(parallelProjMat, element.pos);
    output.Append(element);
}

float4 PSmain(GSOutput input) : SV_TARGET
{
    return input.color;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}