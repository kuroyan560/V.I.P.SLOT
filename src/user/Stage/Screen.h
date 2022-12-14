#pragma once
#include<memory>
class ModelObject;
class RenderTarget;
class TextureBuffer;
class LightManager;
class Camera;

class Screen
{
	//スクリーンのレンダーターゲット
	std::shared_ptr<RenderTarget>m_renderTarget;
	//何も映っていない状態のときのテクスチャ
	std::shared_ptr<TextureBuffer>m_clearTex;
	//描画フラグ
	bool m_draw;

public:
	//モデルオブジェクト
	std::shared_ptr<ModelObject>m_modelObj;

	Screen();

	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//レンダーターゲットクリア
	void ClearTarget();
	//レンダーターゲットセット
	void SetTarget();
	//描画フラグセッタ
	void SetDrawFlg(bool arg_draw) { m_draw = arg_draw; }

};