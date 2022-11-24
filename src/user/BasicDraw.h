#pragma once
#include<memory>
#include"LightBloomDevice.h"
#include"Transform.h"
#include"Color.h"
class LightManager;
class ModelObject;
class Model;
class Camera;
class CubeMap;
class SpriteMesh;
class GraphicsPipeline;
class ConstantBuffer;

class BasicDraw
{
	struct ToonShaderParameter
	{
		//明るい部分に乗算する色
		Color m_brightMulColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		//暗い部分に乗算する色
		Color m_darkMulColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
		//明るさのしきい値（範囲を持たせている）
		float m_brightThresholdLow = 0.31f;
		float m_brightThresholdRange = 0.0f;
	};
	static ToonShaderParameter s_toonShaderParam;

	struct EdgeShaderParameter
	{
		//エッジカラー
		Color m_color = Color(0.0f, 0.0, 0.0f, 1.0f);
		//エッジ描画の判断をする深度差のしきい値
		float m_depthThreshold = 0.1f;
		float m_pad[3];
		//深度値を比べるテクセルへのUVオフセット（近傍8）
		std::array<Vec2<float>, 8>m_uvOffset;
	};
	static EdgeShaderParameter s_edgeShaderParam;

	static int s_drawCount;

	static std::shared_ptr<GraphicsPipeline>s_drawPipeline;
	static std::vector<std::shared_ptr<ConstantBuffer>>s_drawTransformBuff;
	static std::shared_ptr<ConstantBuffer>s_toonShaderParamBuff;

	//エッジ出力＆描画
	static std::shared_ptr<GraphicsPipeline>s_edgePipeline;
	static std::unique_ptr<SpriteMesh>s_spriteMesh;
	static std::shared_ptr<ConstantBuffer>s_edgeShaderParamBuff;

public:
	static void Awake(Vec2<float>arg_screenSize, int arg_prepareBuffNum = 100);
	static void CountReset() { s_drawCount = 0; }
	static void Draw(LightManager& LigManager, std::weak_ptr<Model>Model, Transform& Transform, Camera& Cam, std::shared_ptr<ConstantBuffer>BoneBuff = nullptr);
	static void Draw(LightManager& LigManager, const std::weak_ptr<ModelObject>ModelObject, Camera& Cam);
	static void DrawEdge(std::shared_ptr<TextureBuffer>arg_depthMap);

	static void ImguiDebug();
};