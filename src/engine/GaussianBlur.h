#pragma once
#include<memory>
#include"D3D12Data.h"
#include<array>
#include"SpriteMesh.h"

class GaussianBlur
{
private:
	static enum PROCESS { X_BLUR, Y_BLUR, FINAL, PROCESS_NUM };
	static const int s_weightNum = 8;
	static void GeneratePipeline(std::array<std::shared_ptr<GraphicsPipeline>, PROCESS_NUM - 1>& DestPipeline, const DXGI_FORMAT& Format);

private:
	//重みテーブル
	float m_weights[s_weightNum];

	//スプライトメッシュ
	std::array<std::unique_ptr<SpriteMesh>, PROCESS_NUM - 1>m_spriteMeshes;

	//重みテーブルの定数バッファ
	std::shared_ptr<ConstantBuffer>m_weightConstBuff;

	//ブラーの結果
	std::array<std::shared_ptr<RenderTarget>, PROCESS_NUM>m_blurResult;

	//パイプライン
	std::array<std::shared_ptr<GraphicsPipeline>, PROCESS_NUM - 1>m_gPipeline;

public:
	//ソース画像 & 結果描画先設定、ぼかし強さ
	GaussianBlur(const Vec2<int>& Size, const DXGI_FORMAT& Format, const float& BlurPower = 8.0f);
	//ボケ具合
	void SetBlurPower(const float& BlurPower);
	//グラフィックスマネージャに登録
	void Register(const std::shared_ptr<TextureBuffer>& SourceTex);

	//結果のテクスチャ取得
	std::shared_ptr<RenderTarget>& GetResultTex() { return m_blurResult[FINAL]; }

	//ウィンドウサイズで結果を描画
	void DrawResult(const AlphaBlendMode& AlphaBlend);
};