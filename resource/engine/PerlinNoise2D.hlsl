cbuffer cbuff0 : register(b0)
{
    float2 rectLength;
    int2 split;
    int interpolation;
    int contrast;
    int octaves;
    float baseFrequency;
    float persistance;
}
StructuredBuffer<float2> grads : register(t0);
RWTexture2D<float4> pixels : register(u0);

float Wavelet(float t)
{
    return 1 - (6 * abs(pow(t, 5)) - 15 * pow(t, 4) + 10 * abs(pow(t, 3)));
}

//���z��ݒ肵���E�F�[�u���b�g�֐�
float GradWaveLet(float2 uv, float2 grad)
{
    float l = dot(uv, grad);
    float c = Wavelet(uv.x) * Wavelet(uv.y);
    return c * l;
}

float PerlinNoise(float2 pixelIdx)
{
    float2 imgSize = rectLength * split;
    pixelIdx %= imgSize;    //�J��Ԃ�
    
      //���g�����������`�̊e�p�̃C���f�b�N�X�擾
    int x0Idx = pixelIdx.x / rectLength.x;
    int x1Idx = x0Idx + 1;
    int y0Idx = pixelIdx.y / rectLength.y;
    int y1Idx = y0Idx + 1;
    
    //�e�p�̌��z�x�N�g���擾
    float2 grad_LU = grads[y0Idx * (split.x + 1) + x0Idx];
    float2 grad_LB = grads[y1Idx * (split.x + 1) + x0Idx];
    float2 grad_RU = grads[y0Idx * (split.x + 1) + x1Idx];
    float2 grad_RB = grads[y1Idx * (split.x + 1) + x1Idx];
    
    //���g�����������`�̊e�p�̍��W�擾
    float x0Pos = x0Idx * rectLength.x;
    float x1Pos = x1Idx * rectLength.x;
    float y0Pos = y0Idx * rectLength.y;
    float y1Pos = y1Idx * rectLength.y;
    
    //�e�p�ɑ΂��Ă̑��΍��W
    float2 uv_LU = (pixelIdx - float2(x0Pos, y0Pos)) / rectLength;
    float2 uv_RU = (pixelIdx - float2(x1Pos, y0Pos)) / rectLength;
    float2 uv_LB = (pixelIdx - float2(x0Pos, y1Pos)) / rectLength;
    float2 uv_RB = (pixelIdx - float2(x1Pos, y1Pos)) / rectLength;
    
    //���g�����������`��ł̑��΍��W(�����j
    float2 uvOnSplit = uv_LU;
    
    
    float result = 0.0f;
    
    //���
    if (interpolation == 0)  //Wavelet
    {
        //����ƉE��̑΂ŕ��
        float w_LU = GradWaveLet(uv_LU, grad_LU);
        float w_RU = GradWaveLet(uv_RU, grad_RU);
        float w_U = lerp(w_LU, w_RU, uvOnSplit.x);
    
        //�����ƉE���̑΂ŕ��
        float w_LB = GradWaveLet(uv_LB, grad_LB);
        float w_RB = GradWaveLet(uv_RB, grad_RB);
        float w_B = lerp(w_LB, w_RB, uvOnSplit.x);
    
        //Y�������ɕ��
        result = lerp(w_U, w_B, uvOnSplit.y);
    }
    else if (interpolation == 1) //Block
    {
        //�S�ċ�`�̒��S�_�ŕ��
        float w_LU = dot(grad_LU, float2(0.5f, 0.5f));
        float w_RU = dot(grad_RU, float2(0.5f, 0.5f));
        float w_U = lerp(w_LU, w_RU, 0.5f);
    
        float w_LB = dot(grad_LB, float2(0.5f, 0.5f));
        float w_RB = dot(grad_RB, float2(0.5f, 0.5f));
        float w_B = lerp(w_LB, w_RB, 0.5f);
    
        result = lerp(w_U, w_B, 0.5f);
    }
    
    //�R���g���X�g���グ��
    result = atan(contrast * result);
    
    return result;
}

[numthreads(16, 16, 1)]
void CSmain(uint2 DTid : SV_DispatchThreadID)
{
    float total = 0;
    float frequency = baseFrequency;
    float amplitude = 1;
    float maxValue = 0;
    for (int i = 0; i < octaves;++i)
    {
        total += PerlinNoise(DTid * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistance;
        frequency *= 2;
    }
    
    float result = total / maxValue;
    result = clamp(result, -1.0f, 1.0f);
    pixels[DTid] = float4(result, result, result, result);
};