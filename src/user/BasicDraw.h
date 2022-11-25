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
	//トゥーンシェーダーの共有パラメータ
	struct ToonCommonParameter
	{
		//明るさのしきい値（範囲を持たせている）
		float m_brightThresholdLow = 0.31f;
		float m_brightThresholdRange = 0.0f;
		//リムライトの影響部分をそのままの色で出力する際のしきい値
		float m_limThreshold = 0.0f;
	};
	static ToonCommonParameter s_toonCommonParam;

	//トゥーンシェーダーの個別のパラメータ
	struct ToonIndividualParameter
	{
		//明るい部分に乗算する色
		Color m_brightMulColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
		//暗い部分に乗算する色
		Color m_darkMulColor = Color(0.3f, 0.3f, 0.3f, 1.0f);
	};
	//描画関数呼び出し側から特に指定がなければ使用するパラメータ
	static ToonIndividualParameter s_toonDefaultIndividualParam;

	//エッジの共有パラメータ
	struct EdgeCommonParameter
	{
		//エッジカラー
		Color m_color = Color(0.0f, 0.0, 0.0f, 1.0f);
		//エッジ描画の判断をする深度差のしきい値
		float m_depthThreshold = 0.1f;
		float m_pad[3];
		//深度値を比べるテクセルへのUVオフセット（近傍8）
		std::array<Vec2<float>, 8>m_uvOffset;
	};
	static EdgeCommonParameter s_edgeShaderParam;

	static int s_drawCount;

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
	/// <param name="LigManager">ライトマネージャ</param>
	/// <param name="Model">モデル</param>
	/// <param name="Transform">トランスフォーム</param>
	/// <param name="Cam">カメラ</param>
	/// <param name="arg_toonParam">トゥーンのパラメータ</param>
	/// <param name="BoneBuff">ボーンバッファ</param>
	static void Draw(LightManager& LigManager, std::weak_ptr<Model>Model, Transform& Transform, Camera& Cam, const ToonIndividualParameter& arg_toonParam, std::shared_ptr<ConstantBuffer>BoneBuff = nullptr);

	//描画（デフォルトのトゥーンパラメータを使用）
	static void Draw(LightManager& LigManager, std::weak_ptr<Model>Model, Transform& Transform, Camera& Cam, std::shared_ptr<ConstantBuffer>BoneBuff = nullptr);
	//描画（モデルオブジェクト、トゥーンパラメータ指定）
	static void Draw(LightManager& LigManager, const std::weak_ptr<ModelObject>ModelObject, Camera& Cam, const ToonIndividualParameter& arg_toonParam);
	//描画（モデルオブジェクト、デフォルトのトゥーンパラメータを使用）
	static void Draw(LightManager& LigManager, const std::weak_ptr<ModelObject>ModelObject, Camera& Cam);

	/// <summary>
	/// エッジ描画
	/// </summary>
	/// <param name="arg_depthMap">深度マップ</param>
	static void DrawEdge(std::shared_ptr<TextureBuffer>arg_depthMap);

	static void ImguiDebug();
};