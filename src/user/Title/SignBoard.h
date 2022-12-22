#pragma once
#include<memory>
class RenderTarget;
class TextureBuffer;
class ModelObject;
#include"CRT.h"

class SignBoard
{
	//スクリーンのレンダーターゲット
	std::shared_ptr<RenderTarget>m_renderTarget;
	//何も映っていない状態のときのテクスチャ
	std::shared_ptr<TextureBuffer>m_clearTex;

	//タイトルテクスチャ
	std::shared_ptr<TextureBuffer>m_titleTex;

	//CRTポストエフェクト
	CRT m_crt;

public:
	//モデルオブジェクト
	std::shared_ptr<ModelObject> m_modelObj;

	SignBoard();

	//画面の映像更新
	void UpdateScreen();

	//画面の映像テクスチャゲッタ
	std::shared_ptr<TextureBuffer> GetScreenTex();
};

