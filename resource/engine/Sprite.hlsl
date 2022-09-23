//�ʏ�̃X�v���C�g�`��

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //���s���e�s��
};

cbuffer cbuff1 : register(b1)
{
    matrix mat; //�R�c�ϊ��s��
    float4 color; //�F(RGBA)
}

Texture2D<float4> tex : register(t0); //�O�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); //�O�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

//���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float2 uv : TEXCOORD; //uv�l
};

//���_�V�F�[�_�[
VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.svpos = mul(mat, pos);
    output.svpos = mul(parallelProjMat, output.svpos);
    pos.z = 0.0f;
    output.uv = uv;
    return output;
}

//�s�N�Z���V�F�[�_�[
float4 PSmain(VSOutput input) : SV_TARGET
{
    return tex.Sample(smp, input.uv) * color;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}