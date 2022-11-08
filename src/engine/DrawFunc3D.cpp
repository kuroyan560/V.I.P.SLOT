#include "DrawFunc3D.h"
#include"KuroEngine.h"
#include"Model.h"
#include"LightManager.h"
#include"CubeMap.h"
#include"ModelAnimator.h"

//DrawLine
int DrawFunc3D::s_drawLineCount = 0;
std::map<DXGI_FORMAT, std::array<std::shared_ptr<GraphicsPipeline>, AlphaBlendModeNum>>DrawFunc3D::s_drawLinePipeline;

//DrawNonShadingModel
int DrawFunc3D::s_drawNonShadingCount = 0;
int DrawFunc3D::s_drawNonShadingCountHuge = 0;
//DrawADSShadingModel
int DrawFunc3D::s_drawAdsShadingCount = 0;
//DrawPBRShadingModel
int DrawFunc3D::s_drawPbrShadingCount = 0;
//DrawToonModel
int DrawFunc3D::s_drawToonCount = 0;
//DrawShadowFallModel
int DrawFunc3D::s_drawShadowFallCount = 0;


void DrawFunc3D::GenerateDrawLinePipeline(DXGI_FORMAT arg_format, const AlphaBlendMode& arg_blendMode)
{
	//パイプライン未生成
	if (!s_drawLinePipeline[arg_format][arg_blendMode])
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.m_calling = D3D12_CULL_MODE_NONE;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawLine3D.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_gs = D3D12App::Instance()->CompileShader("resource/engine/DrawLine3D.hlsl", "GSmain", "gs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawLine3D.hlsl", "PSmain", "ps_6_4");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("FROM_POS",DXGI_FORMAT_R32G32B32_FLOAT),
			InputLayoutParam("TO_POS",DXGI_FORMAT_R32G32B32_FLOAT),
			InputLayoutParam("COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
			InputLayoutParam("THICKNESS",DXGI_FORMAT_R32_FLOAT),
		};

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"カメラ情報バッファ"),
		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(arg_format, arg_blendMode) };
		//パイプライン生成
		s_drawLinePipeline[arg_format][arg_blendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });
	}
}

void DrawFunc3D::DrawLine(Camera& arg_cam, const Vec3<float>& arg_from, const Vec3<float>& arg_to, const Color& arg_lineColor, const float& arg_thickness, const AlphaBlendMode& arg_blendMode)
{
	static std::vector<std::shared_ptr<VertexBuffer>>LINE_VERTEX_BUFF;

	const auto targetFormat = KuroEngine::Instance()->Graphics().GetRecentRenderTargetFormat(0);

	//DrawLine専用頂点
	class LineVertex
	{
	public:
		Vec3<float>fromPos;
		Vec3<float>toPos;
		Color color;
		float thickness;
		LineVertex(const Vec3<float>& arg_fromPos, const Vec3<float>& arg_toPos, const Color& arg_color, const float& arg_thickness)
			:fromPos(arg_fromPos), toPos(arg_toPos), color(arg_color), thickness(arg_thickness) {}
	};

	GenerateDrawLinePipeline(targetFormat, arg_blendMode);

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_drawLinePipeline[targetFormat][arg_blendMode]);

	if (LINE_VERTEX_BUFF.size() < (s_drawLineCount + 1))
	{
		LINE_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(LineVertex), 1, nullptr, ("DrawLine3D -" + std::to_string(s_drawLineCount)).c_str()));
	}

	LineVertex vertex(arg_from, arg_to, arg_lineColor, arg_thickness);
	LINE_VERTEX_BUFF[s_drawLineCount]->Mapping(&vertex);
	Vec3<float>center = arg_from.GetCenter(arg_to);

	KuroEngine::Instance()->Graphics().ObjectRender(
		LINE_VERTEX_BUFF[s_drawLineCount],
		{
			{arg_cam.GetBuff(),CBV}
		},
		center.z,
		true);

	s_drawLineCount++;
}

void DrawFunc3D::DrawNonShadingModel(const std::weak_ptr<Model> arg_model, Transform& arg_transform, Camera& arg_cam, const float& arg_alpha, std::shared_ptr<ModelAnimator> arg_animator, const AlphaBlendMode& arg_blendMode)
{
	static std::map<DXGI_FORMAT, std::array<std::shared_ptr<GraphicsPipeline>, AlphaBlendModeNum>>PIPELINE;
	static std::vector<std::shared_ptr<ConstantBuffer>>DRWA_DATA_BUFF;

	struct DrawData
	{
		Matrix m_transformMat;
		float m_alpha = 1.0f;
	};

	const auto targetFormat = KuroEngine::Instance()->Graphics().GetRecentRenderTargetFormat(0);

	//パイプライン未生成
	if (!PIPELINE[targetFormat][arg_blendMode])
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawNonShadingModel.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawNonShadingModel.hlsl", "PSmain", "ps_6_4");

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"カメラ情報バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"描画データバッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"ボーン行列バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"カラーテクスチャ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マテリアル基本情報バッファ"),
		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(targetFormat, arg_blendMode) };
		//パイプライン生成
		PIPELINE[targetFormat][arg_blendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, {WrappedSampler(true, true)});
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[targetFormat][arg_blendMode]);

	if (DRWA_DATA_BUFF.size() < (s_drawNonShadingCount + 1))
	{
		DRWA_DATA_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(DrawData), 1, nullptr, ("DrawShadingModel_DrawData -" + std::to_string(s_drawNonShadingCount)).c_str()));
	}

	DrawData drawData;
	drawData.m_transformMat = arg_transform.GetWorldMat();
	drawData.m_alpha = arg_alpha;
	DRWA_DATA_BUFF[s_drawNonShadingCount]->Mapping(&drawData);

	auto model = arg_model.lock();
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (arg_animator)boneBuff = arg_animator->GetBoneMatBuff();

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{arg_cam.GetBuff(),CBV},
				{DRWA_DATA_BUFF[s_drawNonShadingCount],CBV},
				{boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->buff,CBV}
			},
			arg_transform.GetPos().z,
			arg_blendMode == AlphaBlendMode_Trans);
	}

	s_drawNonShadingCount++;
}

void DrawFunc3D::DrawNonShadingModel(const std::weak_ptr<Model> arg_model, const std::vector<Matrix>& arg_matArray, Camera& arg_cam, const AlphaBlendMode& arg_blendMode, const float& arg_depth, std::shared_ptr<ModelAnimator> arg_animator)
{
	if (INSTANCE_MAX < arg_matArray.size())assert(0);

	static std::map<DXGI_FORMAT, std::array<std::shared_ptr<GraphicsPipeline>, AlphaBlendModeNum>>PIPELINE;
	static std::vector<std::shared_ptr<ConstantBuffer>>TRANSFORM_BUFF;

	const auto targetFormat = KuroEngine::Instance()->Graphics().GetRecentRenderTargetFormat(0);

	//パイプライン未生成
	if (!PIPELINE[targetFormat][arg_blendMode])
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawNonShadingModel_Huge.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawNonShadingModel_Huge.hlsl", "PSmain", "ps_6_4");

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"カメラ情報バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"描画データバッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"ボーン行列バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"カラーテクスチャ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マテリアル基本情報バッファ"),
		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(targetFormat, arg_blendMode) };
		//パイプライン生成
		PIPELINE[targetFormat][arg_blendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(true, true) });
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[targetFormat][arg_blendMode]);

	if (TRANSFORM_BUFF.size() < (s_drawNonShadingCountHuge + 1))
	{
		TRANSFORM_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), INSTANCE_MAX, nullptr, ("DrawShadingModel_Huge_Transform -" + std::to_string(s_drawNonShadingCountHuge)).c_str()));
	}

	TRANSFORM_BUFF[s_drawNonShadingCountHuge]->Mapping(arg_matArray.data(), static_cast<int>(arg_matArray.size()));

	auto model = arg_model.lock();
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (arg_animator)boneBuff = arg_animator->GetBoneMatBuff();

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{arg_cam.GetBuff(),CBV},
				{TRANSFORM_BUFF[s_drawNonShadingCountHuge],CBV},
				{boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->buff,CBV}
			},
			arg_depth,
			arg_blendMode == AlphaBlendMode_Trans,
			static_cast<int>(arg_matArray.size()));
	}

	s_drawNonShadingCountHuge++;
}

void DrawFunc3D::DrawADSShadingModel(LightManager& arg_ligManager, const std::weak_ptr<Model> arg_model, Transform& arg_transform, Camera& arg_cam, std::shared_ptr<ModelAnimator> arg_animator, const AlphaBlendMode& arg_blendMode)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	static std::vector<std::shared_ptr<ConstantBuffer>>TRANSFORM_BUFF;

	//パイプライン未生成
	if (!PIPELINE[arg_blendMode])
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawADSShadingModel.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawADSShadingModel.hlsl", "PSmain", "ps_6_4");

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
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"カラーテクスチャ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ノーマルマップ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マテリアル基本情報バッファ"),

		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), arg_blendMode) };
		//パイプライン生成
		PIPELINE[arg_blendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, {WrappedSampler(false, false)});
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[arg_blendMode]);

	if (TRANSFORM_BUFF.size() < (s_drawAdsShadingCount + 1))
	{
		TRANSFORM_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("DrawADSShadingModel_Transform -" + std::to_string(s_drawAdsShadingCount)).c_str()));
	}

	TRANSFORM_BUFF[s_drawAdsShadingCount]->Mapping(&arg_transform.GetWorldMat());

	auto model = arg_model.lock();
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (arg_animator)boneBuff = arg_animator->GetBoneMatBuff();
	
	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{arg_cam.GetBuff(),CBV},
				{arg_ligManager.GetLigNumInfo(),CBV},
				{arg_ligManager.GetLigInfo(Light::DIRECTION),SRV},
				{arg_ligManager.GetLigInfo(Light::POINT),SRV},
				{arg_ligManager.GetLigInfo(Light::SPOT),SRV},
				{arg_ligManager.GetLigInfo(Light::HEMISPHERE),SRV},
				{TRANSFORM_BUFF[s_drawAdsShadingCount],CBV},
				{boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->texBuff[NORMAL_TEX],SRV},
				{mesh.material->buff,CBV}
			},
			arg_transform.GetPos().z,
			true);
	}

	s_drawAdsShadingCount++;
}

void DrawFunc3D::DrawPBRShadingModel(LightManager& arg_ligManager, const std::weak_ptr<Model> arg_model, Transform& arg_transform, Camera& arg_cam, std::shared_ptr<ModelAnimator> arg_animator, std::shared_ptr<CubeMap>arg_attachCubeMap, const AlphaBlendMode& arg_blendMode)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	static std::vector<std::shared_ptr<ConstantBuffer>>TRANSFORM_BUFF;

	//パイプライン未生成
	if (!PIPELINE[arg_blendMode])
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawPBRShadingModel.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawPBRShadingModel.hlsl", "PSmain", "ps_6_4");

		//ルートパラメータ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"カメラ情報バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "アクティブ中のライト数バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "ディレクションライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "ポイントライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "スポットライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "天球ライト情報 (構造化バッファ)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "キューブマップ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"トランスフォームバッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"ボーン行列バッファ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ベースカラーテクスチャ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"メタルネステクスチャ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ノーマルマップ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"粗さ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マテリアル基本情報バッファ"),

		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), arg_blendMode) };
		//パイプライン生成
		PIPELINE[arg_blendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, {WrappedSampler(false, false)});
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[arg_blendMode]);

	if (TRANSFORM_BUFF.size() < (s_drawPbrShadingCount + 1))
	{
		TRANSFORM_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("DrawPBRShadingModel_Transform -" + std::to_string(s_drawPbrShadingCount)).c_str()));
	}

	TRANSFORM_BUFF[s_drawPbrShadingCount]->Mapping(&arg_transform.GetWorldMat());


	//ボーン行列バッファ取得（アニメーターがnullptrなら空）
	auto model = arg_model.lock();
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (arg_animator)boneBuff = arg_animator->GetBoneMatBuff();

	//キューブマップ（nullptrならデフォルトの静的キューブマップ）
	std::shared_ptr<CubeMap>cubeMap = StaticallyCubeMap::GetDefaultCubeMap();
	if (arg_attachCubeMap)cubeMap = arg_attachCubeMap;

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{arg_cam.GetBuff(),CBV},
				{arg_ligManager.GetLigNumInfo(),CBV},
				{arg_ligManager.GetLigInfo(Light::DIRECTION),SRV},
				{arg_ligManager.GetLigInfo(Light::POINT),SRV},
				{arg_ligManager.GetLigInfo(Light::SPOT),SRV},
				{arg_ligManager.GetLigInfo(Light::HEMISPHERE),SRV},
				{cubeMap->GetCubeMapTex(),SRV},
				{TRANSFORM_BUFF[s_drawPbrShadingCount],CBV},
				{boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->texBuff[METALNESS_TEX],SRV},
				{mesh.material->texBuff[NORMAL_TEX],SRV},
				{mesh.material->texBuff[ROUGHNESS_TEX],SRV},
				{mesh.material->buff,CBV},
			},
			arg_transform.GetPos().z,
			true);
	}

	s_drawPbrShadingCount++;
}

void DrawFunc3D::DrawToonModel(const std::weak_ptr<TextureBuffer> arg_toonTex, LightManager& arg_ligManager, const std::weak_ptr<Model> arg_model, Transform& arg_transform, Camera& arg_cam, const AlphaBlendMode& arg_blendMode)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	static std::vector<std::shared_ptr<ConstantBuffer>>TRANSFORM_BUFF;

	//パイプライン未生成
	if (!PIPELINE[arg_blendMode])
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawToonModel.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawToonModel.hlsl", "PSmain", "ps_6_4");

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
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"カラーテクスチャ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ノーマルマップ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "トゥーンテクスチャ"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マテリアル基本情報バッファ"),

		};

		//レンダーターゲット描画先情報
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), arg_blendMode) };
		//パイプライン生成
		PIPELINE[arg_blendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, {WrappedSampler(false, false)});
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[arg_blendMode]);

	if (TRANSFORM_BUFF.size() < (s_drawToonCount + 1))
	{
		TRANSFORM_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("DrawShadingModel_Transform -" + std::to_string(s_drawToonCount)).c_str()));
	}

	TRANSFORM_BUFF[s_drawToonCount]->Mapping(&arg_transform.GetWorldMat());

	auto model = arg_model.lock();

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{arg_cam.GetBuff(),CBV},
				{arg_ligManager.GetLigNumInfo(),CBV},
				{arg_ligManager.GetLigInfo(Light::DIRECTION),SRV},
				{arg_ligManager.GetLigInfo(Light::POINT),SRV},
				{arg_ligManager.GetLigInfo(Light::SPOT),SRV},
				{arg_ligManager.GetLigInfo(Light::HEMISPHERE),SRV},
				{TRANSFORM_BUFF[s_drawToonCount],CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->texBuff[NORMAL_TEX],SRV},
				{arg_toonTex.lock(),SRV},
				{mesh.material->buff,CBV}
			},
			arg_transform.GetPos().z,
			true);
	}

	s_drawToonCount++;
}