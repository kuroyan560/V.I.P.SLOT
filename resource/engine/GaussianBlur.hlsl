
struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 tex0 : TEXCOORD0;
    float4 tex1 : TEXCOORD1;
    float4 tex2 : TEXCOORD2;
    float4 tex3 : TEXCOORD3;
    float4 tex4 : TEXCOORD4;
    float4 tex5 : TEXCOORD5;
    float4 tex6 : TEXCOORD6;
    float4 tex7 : TEXCOORD7;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);
cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat;
}
cbuffer cbuff1 : register(b1)
{
    float4 weight[2];
};

VSOutput HorizontalMain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.pos = mul(parallelProjMat, pos);
    pos.z = 0.0f;
    
    //�e�N�X�`���T�C�Y�擾
    float2 texSize;
    float level;
    tex.GetDimensions(0, texSize.x, texSize.y, level);
    
    //��e�N�Z����UV���W���L�^
    float2 tex = uv;
    output.tex0.xy = uv;
    //return output;
    
    // ��e�N�Z������U���W��+1�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex0.xy = float2(1.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+3�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex1.xy = float2(3.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+5�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex2.xy = float2(5.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+7�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex3.xy = float2(7.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+9�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex4.xy = float2(9.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+11�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex5.xy = float2(11.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+13�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex6.xy = float2(13.0f / texSize.x, 0.0f);

    // ��e�N�Z������U���W��+15�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex7.xy = float2(15.0f / texSize.x, 0.0f);

    // �I�t�Z�b�g��-1���|���ă}�C�i�X�����̃I�t�Z�b�g���v�Z����
    output.tex0.zw = output.tex0.xy * -1.0f;
    output.tex1.zw = output.tex1.xy * -1.0f;
    output.tex2.zw = output.tex2.xy * -1.0f;
    output.tex3.zw = output.tex3.xy * -1.0f;
    output.tex4.zw = output.tex4.xy * -1.0f;
    output.tex5.zw = output.tex5.xy * -1.0f;
    output.tex6.zw = output.tex6.xy * -1.0f;
    output.tex7.zw = output.tex7.xy * -1.0f;

    // �I�t�Z�b�g�Ɋ�e�N�Z����UV���W�𑫂��Z���āA
    // ���ۂɃT���v�����O����UV���W�ɕϊ�����
    output.tex0 += float4(tex, tex);
    output.tex1 += float4(tex, tex);
    output.tex2 += float4(tex, tex);
    output.tex3 += float4(tex, tex);
    output.tex4 += float4(tex, tex);
    output.tex5 += float4(tex, tex);
    output.tex6 += float4(tex, tex);
    output.tex7 += float4(tex, tex);

    return output;
}

VSOutput VerticalMain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; //�s�N�Z���V�F�[�_�ɓn���l
    output.pos = mul(parallelProjMat, pos);
    pos.z = 0.0f;
    
    //�e�N�X�`���T�C�Y�擾
    float2 texSize;
    float level;
    tex.GetDimensions(0, texSize.x, texSize.y, level);
    
     // ��e�N�Z����UV���W���L�^
    float2 tex = uv;
    output.tex0.xy = uv;
    //return output;
    
    // ��e�N�Z������V���W��+1�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex0.xy = float2(0.0f, 1.0f / texSize.y);

    // ��e�N�Z������V���W��+3�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex1.xy = float2(0.0f, 3.0f / texSize.y);

    // ��e�N�Z������V���W��+5�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex2.xy = float2(0.0f, 5.0f / texSize.y);

    // ��e�N�Z������V���W��+7�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex3.xy = float2(0.0f, 7.0f / texSize.y);

    // ��e�N�Z������V���W��+9�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex4.xy = float2(0.0f, 9.0f / texSize.y);

    // ��e�N�Z������V���W��+11�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex5.xy = float2(0.0f, 11.0f / texSize.y);

    // ��e�N�Z������V���W��+13�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex6.xy = float2(0.0f, 13.0f / texSize.y);

    // ��e�N�Z������V���W��+15�e�N�Z�����炷���߂̃I�t�Z�b�g���v�Z����
    output.tex7.xy = float2(0.0f, 15.0f / texSize.y);

    // �I�t�Z�b�g��-1���|���ă}�C�i�X�����̃I�t�Z�b�g���v�Z����
    output.tex0.zw = output.tex0.xy * -1.0f;
    output.tex1.zw = output.tex1.xy * -1.0f;
    output.tex2.zw = output.tex2.xy * -1.0f;
    output.tex3.zw = output.tex3.xy * -1.0f;
    output.tex4.zw = output.tex4.xy * -1.0f;
    output.tex5.zw = output.tex5.xy * -1.0f;
    output.tex6.zw = output.tex6.xy * -1.0f;
    output.tex7.zw = output.tex7.xy * -1.0f;

    // �I�t�Z�b�g�Ɋ�e�N�Z����UV���W�𑫂��Z���āA
    // ���ۂɃT���v�����O����UV���W�ɕϊ�����
    output.tex0 += float4(tex, tex);
    output.tex1 += float4(tex, tex);
    output.tex2 += float4(tex, tex);
    output.tex3 += float4(tex, tex);
    output.tex4 += float4(tex, tex);
    output.tex5 += float4(tex, tex);
    output.tex6 += float4(tex, tex);
    output.tex7 += float4(tex, tex);

    return output;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 result;
    
    //return tex.Sample(smp, input.tex0.xy);
    
    // ��e�N�Z������v���X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    result = weight[0].x * tex.Sample(smp, input.tex0.xy);
    result += weight[0].y * tex.Sample(smp, input.tex1.xy);
    result += weight[0].z * tex.Sample(smp, input.tex2.xy);
    result += weight[0].w * tex.Sample(smp, input.tex3.xy);
    result += weight[1].x * tex.Sample(smp, input.tex4.xy);
    result += weight[1].y * tex.Sample(smp, input.tex5.xy);
    result += weight[1].z * tex.Sample(smp, input.tex6.xy);
    result += weight[1].w * tex.Sample(smp, input.tex7.xy);

    // ��e�N�Z���Ƀ}�C�i�X������8�e�N�Z���A�d�ݕt���ŃT���v�����O
    result += weight[0].x * tex.Sample(smp, input.tex0.zw);
    result += weight[0].y * tex.Sample(smp, input.tex1.zw);
    result += weight[0].z * tex.Sample(smp, input.tex2.zw);
    result += weight[0].w * tex.Sample(smp, input.tex3.zw);
    result += weight[1].x * tex.Sample(smp, input.tex4.zw);
    result += weight[1].y * tex.Sample(smp, input.tex5.zw);
    result += weight[1].z * tex.Sample(smp, input.tex6.zw);
    result += weight[1].w * tex.Sample(smp, input.tex7.zw);
    
    return result;
}