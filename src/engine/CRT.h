#pragma once
#include"Vec.h"
#include<memory>
#include"D3D12Data.h"
#include<map>
class SpriteMesh;

class CRT
{
	struct Info
	{
		//画面サイズ
		Vec2<float>m_screenSize;
		//画面歪み強さ
		float m_screenDistort = 0.1f;
		//ガウシアンフィルタをかけてサンプリングする際の重み
		float gaussianSampleWeight = 16.0f;
	}m_crtInfo;

	std::shared_ptr<SpriteMesh>m_spriteMesh;
	std::shared_ptr<ConstantBuffer>m_crtInfoBuff;
	std::shared_ptr<RenderTarget>m_resultTex;

public:
	CRT();

	void Update();

	//グラフィックスマネージャに登録
	void Register(const std::shared_ptr<TextureBuffer>& arg_srcTex);
	//結果のテクスチャ取得
	std::shared_ptr<RenderTarget>& GetResultTex() { return m_resultTex; }
	//ウィンドウサイズで結果を描画
	void DrawResult(const AlphaBlendMode& AlphaBlend);
};