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
std::shared_ptr<ConstantBuffer>BasicDraw::s_parametersBuff;

std::shared_ptr<GraphicsPipeline>BasicDraw::s_edgePipeline;
std::unique_ptr<SpriteMesh>BasicDraw::s_spriteMesh;

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

		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Trans),	//通常描画
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT, AlphaBlendMode_Trans),	//エミッシブマップ
			RenderTargetInfo(DXGI_FORMAT_R32_FLOAT, AlphaBlendMode_None),	//深度マップ
		};
		//パイプライン生成
		s_drawPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(true, true) });
	}

	for (int i = 0; i < arg_prepareBuffNum; ++i)
		s_drawTransformBuff.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("BasicDraw - Transform -" + std::to_string(i)).c_str()));

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
}

void BasicDraw::Draw(LightManager& LigManager, std::weak_ptr<Model>Model, Transform& Transform, Camera& Cam, std::shared_ptr<ConstantBuffer>BoneBuff)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_drawPipeline);

	if (s_drawTransformBuff.size() < (s_drawCount + 1))
	{
		s_drawTransformBuff.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("BasicDraw - Transform -" + std::to_string(s_drawCount)).c_str()));
	}

	s_drawTransformBuff[s_drawCount]->Mapping(&Transform.GetWorldMat());

	auto model = Model.lock();

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{Cam.GetBuff(),CBV},
				{LigManager.GetLigNumInfo(),CBV},
				{LigManager.GetLigInfo(Light::DIRECTION),SRV},
				{LigManager.GetLigInfo(Light::POINT),SRV},
				{LigManager.GetLigInfo(Light::SPOT),SRV},
				{LigManager.GetLigInfo(Light::HEMISPHERE),SRV},
				{s_drawTransformBuff[s_drawCount],CBV},
				{BoneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->buff,CBV}
			},
			Transform.GetPos().z,
			true);
	}

	s_drawCount++;
}

void BasicDraw::Draw(LightManager& LigManager, const std::weak_ptr<ModelObject> ModelObject, Camera& Cam)
{
	auto obj = ModelObject.lock();
	//ボーン行列バッファ取得（アニメーターがnullptrなら空）
	auto model = obj->m_model;
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (obj->m_animator)boneBuff = obj->m_animator->GetBoneMatBuff();

	Draw(LigManager, model, obj->m_transform, Cam, boneBuff);
}

void BasicDraw::DrawEdge(std::shared_ptr<TextureBuffer> arg_depthMap)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_edgePipeline);

	std::vector<RegisterDescriptorData>descDatas =
	{
		{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
		{arg_depthMap,SRV},
	};
	s_spriteMesh->Render(descDatas);
}
