struct EdgeParameter
{
	//エッジカラー
    float4 m_color;
    //エッジ描画の判断をする深度差のしきい値
    float m_depthThreshold;
    //深度値を比べるテクセルへのUVオフセット
    float2 m_uvOffset[8];
};


struct VSOutput
{
    float4 m_pos : SV_POSITION;
    float2 m_uv : TEXCORRD;
};

Texture2D<float4> g_depthMap : register(t0);
SamplerState g_sampler : register(s0);
cbuffer cbuff0 : register(b0)
{
    matrix parallelProjMat;
}

cbuffer cbuff0 : register(b1)
{
    EdgeParameter m_edgeParam;
}

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    VSOutput output; //ピクセルシェーダに渡す値
    output.m_pos = mul(parallelProjMat, pos);
    pos.z = 0.0f;
    output.m_uv = uv;
    return output;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    // このピクセルの深度値を取得
    float depth = g_depthMap.Sample(g_sampler, input.m_uv).x;

    // 近傍8テクセルの深度値の平均値を計算する
    float depth2 = 0.0f;
    for( int i = 0; i < 8; i++)
    {
        depth2 += g_depthMap.Sample(g_sampler, input.m_uv + m_edgeParam.m_uvOffset[i]).x;
    }
    depth2 /= 8.0f;

    // 自身の深度値と近傍8テクセルの深度値の差を調べる
    // 深度値が結構違う場合はピクセルカラーを黒にする
    if (m_edgeParam.m_depthThreshold <= abs(depth - depth2))
    {
        //エッジ出力
        return m_edgeParam.m_color;
    }
    
    discard;
    return float4(0.0f,0.0f,0.0f,0.0f);
}