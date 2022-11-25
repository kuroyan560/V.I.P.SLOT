#pragma once
#include<memory>
#include"LightBloomDevice.h"
#include"Transform.h"
#include"Color.h"
#include"BasicDrawParameters.h"

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
	//トゥーンシェーダーの共有パラメータ
	struct ToonCommonParameter
	{
		//明るさのしきい値（範囲を持たせている）
		float m_brightThresholdLow = 0.66f;
		float m_brightThresholdRange = 0.03f;
		//リムライトの影響部分をそのままの色で出力する際のしきい値
		float m_limThreshold = 0.4f;
	};
	static ToonCommonParameter s_toonCommonParam;

	//エッジの共有パラメータ
	struct EdgeCommonParameter
	{
		//エッジ描画の判断をする深度差のしきい値
		float m_depthThreshold = 0.1f;
		float m_pad[3];
		//深度値を比べるテクセルへのUVオフセット（近傍8）
		std::array<Vec2<float>, 8>m_uvOffset;
	};
	static EdgeCommonParameter s_edgeShaderParam;

	static int s_drawCount;

	//モデル描画
	static std::shared_ptr<GraphicsPipeline>s_drawPipeline;
	static std::vector<std::shared_ptr<ConstantBuffer>>s_drawTransformBuff;
	static std::vector<std::shared_ptr<ConstantBuffer>>s_toonIndividualParamBuff;
	static std::shared_ptr<ConstantBuffer>s_toonCommonParamBuff;

	//エッジ出力＆描画
	static std::shared_ptr<GraphicsPipeline>s_edgePipeline;
	static std::unique_ptr<SpriteMesh>s_spriteMesh;
	static std::shared_ptr<ConstantBuffer>s_edgeShaderParamBuff;

public:
	static void Awake(Vec2<float>arg_screenSize, int arg_prepareBuffNum = 100);
	static void CountReset() { s_drawCount = 0; }

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="arg_ligMgr">ライトマネージャ</param>
	/// <param name="arg_model">モデル</param>
	/// <param name="arg_transform">トランスフォーム</param>
	/// <param name="arg_cam">カメラ</param>
	/// <param name="arg_toonParam">トゥーンのパラメータ</param>
	/// <param name="arg_boneBuff">ボーンバッファ</param>
	static void Draw(LightManager& arg_ligMgr, std::weak_ptr<Model>arg_model, Transform& arg_transform, Camera& arg_cam, const IndividualDrawParameter& arg_toonParam, std::shared_ptr<ConstantBuffer>arg_boneBuff = nullptr);

	//描画（デフォルトのトゥーンパラメータを使用）
	static void Draw(LightManager& arg_ligMgr, std::weak_ptr<Model>arg_model, Transform& arg_transform, Camera& arg_cam, std::shared_ptr<ConstantBuffer>arg_boneBuff = nullptr);
	//描画（モデルオブジェクト、トゥーンパラメータ指定）
	static void Draw(LightManager& arg_ligMgr, const std::weak_ptr<ModelObject>arg_modelObj, Camera& arg_cam, const IndividualDrawParameter& arg_toonParam);
	//描画（モデルオブジェクト、デフォルトのトゥーンパラメータを使用）
	static void Draw(LightManager& arg_ligMgr, const std::weak_ptr<ModelObject>arg_modelObj, Camera& arg_cam);

	/// <summary>
	/// エッジ描画
	/// </summary>
	/// <param name="arg_depthMap">深度マップ</param>
	/// <param name="arg_edgeColorMap">エッジカラーマップ</param>
	static void DrawEdge(std::shared_ptr<TextureBuffer>arg_depthMap, std::shared_ptr<TextureBuffer>arg_edgeColorMap);

	static void ImguiDebug();
};