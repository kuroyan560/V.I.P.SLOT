cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

//���_�V�F�[�_�[
VSOutput VSmain(float4 pos : POSITION, float4 color : COLOR)
{
    VSOutput output;
    output.pos = mul(parallelProjMat, pos);
    output.color = color;
    return output;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    return input.color;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}