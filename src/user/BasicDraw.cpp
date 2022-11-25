#include "BasicDraw.h"
#include"Model.h"
#include"KuroEngine.h"
#include"Object.h"
#include"ModelAnimator.h"
#include"CubeMap.h"
#include"Camera.h"
#include"LightManager.h"
#include"SpriteMesh.h"

int BasicDraw::s_drawCount = 0;

std::shared_ptr<GraphicsPipeline>BasicDraw::s_drawPipeline;
std::vector<std::shared_ptr<ConstantBuffer>>BasicDraw::s_drawTransformBuff;
std::vector<std::shared_ptr<ConstantBuffer>>BasicDraw::s_toonIndividualParamBuff;
std::shared_ptr<ConstantBuffer>BasicDraw::s_toonCommonParamBuff;
BasicDraw::ToonCommonParameter BasicDraw::s_toonCommonParam;

std::shared_ptr<GraphicsPipeline>BasicDraw::s_edgePipeline;
std::unique_ptr<SpriteMesh>BasicDraw::s_spriteMesh;
std::shared_ptr<ConstantBuffer>BasicDraw::s_edgeShaderParamBuff;
BasicDraw::EdgeCommonParameter BasicDraw::s_edgeShaderParam;

void BasicDraw::Awake(Vec2<float>arg_screenSize, int arg_prepareBuffNum)
{
	s_spriteMesh = std::make_unique<SpriteMesh>("BasicDraw");
	s_spriteMesh->SetSize(arg_screenSize);

	//通常描画パイプライン生成
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/user/shaders/BasicShader.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/user/shaders/BasicShader.hlsl", "PSmain", "ps_6_4");

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"カメラ情報バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "アクティブ中のライト数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "ディレクションライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "ポイントライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "スポットライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "天球ライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"トランスフォームバッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"ボーン行列バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ベースカラーテクスチャ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マテリアル基本情報バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"トゥーンの共通パラメータ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"トゥーンの個別パラメータ"),

		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans),	//通常描画
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT, AlphaBlendMode_Trans),	//エミッシブマップ
			RenderTargetInfo(DXGI_FORMAT_R32_FLOAT, AlphaBlendMode_None),	//深度マップ
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None),	//エッジカラーマップ
		};
		//パイプライン生成
		s_drawPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(
			PIPELINE_OPTION, 
			SHADERS,
			ModelMesh::Vertex::GetInputLayout(), 
			ROOT_PARAMETER, 
			RENDER_TARGET_INFO,
			{ WrappedSampler(true, true) });
	}

	//事前にバッファを用意
	for (int i = 0; i < arg_prepareBuffNum; ++i)
	{
		s_drawTransformBuff.emplace_back(
			D3D12App::Instance()->GenerateConstantBuffer(
				sizeof(Matrix), 
				1, 
				nullptr, 
				("BasicDraw - Transform -" + std::to_string(i)).c_str()));

		s_toonIndividualParamBuff.emplace_back(
			D3D12App::Instance()->GenerateConstantBuffer(
				sizeof(IndividualDrawParameter), 
				1, 
				nullptr, 
				("BasicDraw - IndividualDrawParameter -" + std::to_string(i)).c_str()));
	}

	//トゥーンシェーダー用のバッファを用意
	s_toonCommonParamBuff = D3D12App::Instance()->GenerateConstantBuffer(
		sizeof(s_toonCommonParam),
		1, 
		&s_toonCommonParam, 
		"BasicDraw - ToonCommonParameter");

	//エッジラインパイプライン
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/user/shaders/EdgeShader.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/user/shaders/EdgeShader.hlsl", "PSmain", "ps_6_4");

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"平行投影行列"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"デプスマップ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"エッジカラーマップ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"専用のパラメータ"),
		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None),	//通常描画
		};
		//パイプライン生成
		s_edgePipeline = D3D12App::Instance()->GenerateGraphicsPipeline(
			PIPELINE_OPTION, 
			SHADERS, 
			SpriteMesh::Vertex::GetInputLayout(),
			ROOT_PARAMETER,
			RENDER_TARGET_INFO,
			{ WrappedSampler(false, true) });
	}

	//ウィンドウサイズからUVオフセットを求める
	const auto winSize = WinApp::Instance()->GetWinSize().Float();
	s_edgeShaderParam.m_uvOffset = 
	{
		Vec2<float>(0.0f,  1.0f / winSize.y), //上
		Vec2<float>(0.0f, -1.0f / winSize.y), //下
		Vec2<float>(1.0f / winSize.x,           0.0f), //右
		Vec2<float>(-1.0f / winSize.x,           0.0f), //左
		Vec2<float>(1.0f / winSize.x,  1.0f / winSize.y), //右上
		Vec2<float>(-1.0f / winSize.x,  1.0f / winSize.y), //左上
		Vec2<float>(1.0f / winSize.x, -1.0f / winSize.y), //右下
		Vec2<float>(-1.0f / winSize.x, -1.0f / winSize.y)  //左下
	};

	//エッジ出力用のバッファを用意
	s_edgeShaderParamBuff = D3D12App::Instance()->GenerateConstantBuffer(
		sizeof(s_edgeShaderParam),
		1, 
		&s_edgeShaderParam, 
		"BasicDraw - EdgeCommonParameter");
}

void BasicDraw::Draw(LightManager& arg_ligMgr, std::weak_ptr<Model>arg_model, Transform& arg_transform, Camera& arg_cam, const IndividualDrawParameter& arg_toonParam, std::shared_ptr<ConstantBuffer>arg_boneBuff)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_drawPipeline);

	//トランスフォームバッファ送信
	if (s_drawTransformBuff.size() < (s_drawCount + 1))
	{
		s_drawTransformBuff.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("BasicDraw - Transform -" + std::to_string(s_drawCount)).c_str()));
	}
	s_drawTransformBuff[s_drawCount]->Mapping(&arg_transform.GetWorldMat());

	//トゥーンの個別パラメータバッファ送信
	if (s_toonIndividualParamBuff.size() < (s_drawCount + 1))
	{
		s_toonIndividualParamBuff.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(IndividualDrawParameter), 1, nullptr, ("BasicDraw - IndividualDrawParameter -" + std::to_string(s_drawCount)).c_str()));
	}
	s_toonIndividualParamBuff[s_drawCount]->Mapping(&arg_toonParam);

	auto model = arg_model.lock();

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{arg_cam.GetBuff(),CBV},
				{arg_ligMgr.GetLigNumInfo(),CBV},
				{arg_ligMgr.GetLigInfo(Light::DIRECTION),SRV},
				{arg_ligMgr.GetLigInfo(Light::POINT),SRV},
				{arg_ligMgr.GetLigInfo(Light::SPOT),SRV},
				{arg_ligMgr.GetLigInfo(Light::HEMISPHERE),SRV},
				{s_drawTransformBuff[s_drawCount],CBV},
				{arg_boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->buff,CBV},
				{s_toonCommonParamBuff,CBV},
				{s_toonIndividualParamBuff[s_drawCount],CBV},
			},
			arg_transform.GetPos().z,
			true);
	}

	s_drawCount++;
}

void BasicDraw::Draw(LightManager& arg_ligMgr, std::weak_ptr<Model> arg_model, Transform& arg_transform, Camera& arg_cam, std::shared_ptr<ConstantBuffer> arg_boneBuff)
{
	BasicDraw::Draw(arg_ligMgr, arg_model, arg_transform, arg_cam, IndividualDrawParameter::GetDefault(), arg_boneBuff);
}

void BasicDraw::Draw(LightManager& arg_ligMgr, const std::weak_ptr<ModelObject> arg_modelObj, Camera& arg_cam, const IndividualDrawParameter& arg_toonParam)
{
	auto obj = arg_modelObj.lock();
	//ボーン行列バッファ取得（アニメーターがnullptrなら空）
	auto model = obj->m_model;
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (obj->m_animator)boneBuff = obj->m_animator->GetBoneMatBuff();
	Draw(arg_ligMgr, model, obj->m_transform, arg_cam, arg_toonParam, boneBuff);
}

void BasicDraw::Draw(LightManager& arg_ligMgr, const std::weak_ptr<ModelObject> arg_modelObj, Camera& arg_cam)
{
	Draw(arg_ligMgr, arg_modelObj, arg_cam, IndividualDrawParameter::GetDefault());
}

void BasicDraw::DrawEdge(std::shared_ptr<TextureBuffer> arg_depthMap, std::shared_ptr<TextureBuffer>arg_edgeColorMap)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_edgePipeline);

	std::vector<RegisterDescriptorData>descDatas =
	{
		{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
		{arg_depthMap,SRV},
		{arg_edgeColorMap,SRV},
		{s_edgeShaderParamBuff,CBV},
	};
	s_spriteMesh->Render(descDatas);
}

#include"ImguiApp.h"
void BasicDraw::ImguiDebug()
{
	ImGui::Begin("BasicDraw");

	//描画
	if (ImGui::TreeNode("Toon"))
	{
		//トゥーンシェーダーの共通パラメータ
		bool toonCommonParamChanged = false;
		//しきい値下限
		if (ImGui::DragFloat("BrightThresholdLow", &s_toonCommonParam.m_brightThresholdLow, 0.01f, 0.0f, 1.0f, "%f"))toonCommonParamChanged = true;
		//しきい値範囲
		if (ImGui::DragFloat("BrightThresholdRange", &s_toonCommonParam.m_brightThresholdRange, 0.01f, 0.0f, 1.0f - s_toonCommonParam.m_brightThresholdLow, "%f"))toonCommonParamChanged = true;
		//リムライト強調のしきい値
		if (ImGui::DragFloat("LimThreshold", &s_toonCommonParam.m_limThreshold, 0.01f, 0.0f, 1.0f, "%f"))toonCommonParamChanged = true;
		if (toonCommonParamChanged)s_toonCommonParamBuff->Mapping(&s_toonCommonParam);

		if (ImGui::TreeNode("DefaultIndividualParameter"))
		{
			IndividualDrawParameter::GetDefault().ImguiDebugItem();
			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	//エッジライン
	if (ImGui::TreeNode("Edge"))
	{
		bool edgeParamChanged = false;
		if (ImGui::DragFloat("DepthDifferenceThreshold", &s_edgeShaderParam.m_depthThreshold))edgeParamChanged = true;
		if (edgeParamChanged)s_edgeShaderParamBuff->Mapping(&s_edgeShaderParam);
		ImGui::TreePop();
	}

	ImGui::End();
}
