struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //uv値
};

cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat; //平行投影行列
}

cbuffer cbuff1 : register(b1)
{
    float noiseX;
    float rgbNoise;
    float sinNoiseScale;
    float sinNoiseWidth;
    float sinNoiseOffset;
    float scanLineSpeed;
    float scanLineTail;
    float _time;
    float2 offset;
    float screenParamX;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; //ピクセルシェーダに渡す値
    output.svpos = mul(parallelProjMat, pos);
    pos.z = 0.0f;
    output.uv = uv;
    return output;
}

float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43578.5453);
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    float2 inUV = input.uv;
    float2 uv = input.uv - 0.5f;
    
    //UV座標を再計算し、 画面を歪ませる
    float vignet = length(uv);
    uv /= 1 - vignet * 0.2f;
    float2 texUV = uv + 0.5f;
    
    //画面外なら描画しない
    if (max(abs(uv.y) - 0.5f, abs(uv.x) - 0.5) > 0)
    {
        return float4(0, 0, 0, 1);
    }
    
    //色を計算
    float3 col = float3(0, 0, 0);
    
    //ノイズ、 オフセットを適用
    texUV.x += sin(texUV.y * sinNoiseWidth + sinNoiseOffset) * sinNoiseScale;
    texUV += offset;
    texUV.x += (rand(floor(texUV.y * 500) + _time) - 0.5f) * noiseX;
    
    //色を取得、 RGBを少しずつずらす
    col.x = tex.Sample(smp, texUV).x;
    col.y = tex.Sample(smp, texUV - float2(0.001 / 2.0f, 0)).y;
    col.z = tex.Sample(smp, texUV - float2(0.002 / 2.0f, 0)).z;
    //col = sceneTexture.Sample(smp, texUV);
    
    //RGBノイズ
    if (rand((rand(floor(texUV.y * 500) + _time) - 0.5) + _time) < rgbNoise)
    {
        col.r = rand(uv + float2(123 + _time, 0));
        col.g = rand(uv + float2(123 + _time, 1));
        col.b = rand(uv + float2(123 + _time, 2));
    }
    
    //ピクセルごとに描画するRGBを決める
    //float floorX = fmod(inUV.x * screenParamX / 3, 1);
    //col.r *= floorX > 0.3333;
    //col.g *= floorX < 0.3333 || floorX > 0.6666;
    //col.b *= floorX < 0.6666;
    
    //スキャンラインを描画
    float scanLineColor = sin(_time * 10 + uv.y * 500) / 2 + 0.5f;
    scanLineColor += float3(0.2f, 0.2f, 0.2f);
    col *= 0.5f + clamp(scanLineColor + 0.5, 0, 1) * 0.5;
    
    ////スキャンラインの残像を描画
    //float tail = clamp((frac(uv.y + _time * scanLineSpeed) - 1 + scanLineTail) / min(scanLineTail, 1), 0, 1);
    //col *= tail;
    
    //画面端を暗くする
    //float rate = vignet * 3.0f;
    //rate = clamp(vignet, 0.0f, 1.0f);
    //col *= 1 - rate;
    
    return float4(col, 1);
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}