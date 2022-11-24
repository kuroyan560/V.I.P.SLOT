
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
    VSOutput output; //ピクセルシェーダに渡す値
    output.pos = mul(parallelProjMat, pos);
    pos.z = 0.0f;
    output.uv = uv;
    return output;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    // 近傍8テクセルへのUVオフセット
    float2 uvOffset[8] = 
    {
        float2(           0.0f,  1.0f / 720.0f), //上
        float2(           0.0f, -1.0f / 720.0f), //下
        float2( 1.0f / 1280.0f,           0.0f), //右
        float2(-1.0f / 1280.0f,           0.0f), //左
        float2( 1.0f / 1280.0f,  1.0f / 720.0f), //右上
        float2(-1.0f / 1280.0f,  1.0f / 720.0f), //左上
        float2( 1.0f / 1280.0f, -1.0f / 720.0f), //右下
        float2(-1.0f / 1280.0f, -1.0f / 720.0f)  //左下
    };

    // このピクセルの深度値を取得
    float depth = g_depthMap.Sample(g_sampler, input.uv).x;

    // 近傍8テクセルの深度値の平均値を計算する
    float depth2 = 0.0f;
    for( int i = 0; i < 8; i++)
    {
        depth2 += g_depthMap.Sample(g_sampler, input.uv + uvOffset[i]).x;
    }
    depth2 /= 8.0f;

    //エッジ描画の判断をする深度差のしきい値
    const float depthThreshold = 0.05f;

    //エッジカラー
    const float edgeColor = float4(0.0f,0.0f,0.0f,1.0f);

    // 自身の深度値と近傍8テクセルの深度値の差を調べる
    // 深度値が結構違う場合はピクセルカラーを黒にする
    if(depthThreshold <= abs(depth - depth2))
    {
        //エッジ出力
        return edgeColor;
    }
    
    discard;
    return float4(0.0f,0.0f,0.0f,0.0f);
}