
struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCORRD;
};

Texture2D<float4> g_depthMap : register(t0);
SamplerState g_sampler : register(s0);
cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat;
}

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.pos = mul(parallelProjMat, pos);
    pos.z = 0.0f;
    output.uv = uv;
    return output;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    // �ߖT8�e�N�Z���ւ�UV�I�t�Z�b�g
    float2 uvOffset[8] = 
    {
        float2(           0.0f,  1.0f / 720.0f), //��
        float2(           0.0f, -1.0f / 720.0f), //��
        float2( 1.0f / 1280.0f,           0.0f), //�E
        float2(-1.0f / 1280.0f,           0.0f), //��
        float2( 1.0f / 1280.0f,  1.0f / 720.0f), //�E��
        float2(-1.0f / 1280.0f,  1.0f / 720.0f), //����
        float2( 1.0f / 1280.0f, -1.0f / 720.0f), //�E��
        float2(-1.0f / 1280.0f, -1.0f / 720.0f)  //����
    };

    // ���̃s�N�Z���̐[�x�l���擾
    float depth = g_depthMap.Sample(g_sampler, input.uv).x;

    // �ߖT8�e�N�Z���̐[�x�l�̕��ϒl���v�Z����
    float depth2 = 0.0f;
    for( int i = 0; i < 8; i++)
    {
        depth2 += g_depthMap.Sample(g_sampler, input.uv + uvOffset[i]).x;
    }
    depth2 /= 8.0f;

    //�G�b�W�`��̔��f������[�x���̂������l
    const float depthThreshold = 0.05f;

    //�G�b�W�J���[
    const float edgeColor = float4(0.0f,0.0f,0.0f,1.0f);

    // ���g�̐[�x�l�ƋߖT8�e�N�Z���̐[�x�l�̍��𒲂ׂ�
    // �[�x�l�����\�Ⴄ�ꍇ�̓s�N�Z���J���[�����ɂ���
    if(depthThreshold <= abs(depth - depth2))
    {
        //�G�b�W�o��
        return edgeColor;
    }
    
    discard;
    return float4(0.0f,0.0f,0.0f,0.0f);
}