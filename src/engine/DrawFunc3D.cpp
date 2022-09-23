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
//DrawADSShadingModel
int DrawFunc3D::s_drawAdsShadingCount = 0;
//DrawPBRShadingModel
int DrawFunc3D::s_drawPbrShadingCount = 0;
//DrawToonModel
int DrawFunc3D::s_drawToonCount = 0;
//DrawShadowFallModel
int DrawFunc3D::s_drawShadowFallCount = 0;


void DrawFunc3D::GenerateDrawLinePipeline(DXGI_FORMAT Format, const AlphaBlendMode& BlendMode)
{
	//�p�C�v���C��������
	if (!s_drawLinePipeline[Format][BlendMode])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.m_calling = D3D12_CULL_MODE_NONE;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawLine3D.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_gs = D3D12App::Instance()->CompileShader("resource/engine/DrawLine3D.hlsl", "GSmain", "gs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawLine3D.hlsl", "PSmain", "ps_6_4");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("FROM_POS",DXGI_FORMAT_R32G32B32_FLOAT),
			InputLayoutParam("TO_POS",DXGI_FORMAT_R32G32B32_FLOAT),
			InputLayoutParam("COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
			InputLayoutParam("THICKNESS",DXGI_FORMAT_R32_FLOAT),
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(Format, BlendMode) };
		//�p�C�v���C������
		s_drawLinePipeline[Format][BlendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });
	}
}

void DrawFunc3D::DrawLine(Camera& Cam, const Vec3<float>& From, const Vec3<float>& To, const Color& LineColor, const float& Thickness, const AlphaBlendMode& BlendMode)
{
	static std::vector<std::shared_ptr<VertexBuffer>>LINE_VERTEX_BUFF;

	const auto targetFormat = KuroEngine::Instance()->Graphics().GetRecentRenderTargetFormat(0);

	//DrawLine��p���_
	class LineVertex
	{
	public:
		Vec3<float>fromPos;
		Vec3<float>toPos;
		Color color;
		float thickness;
		LineVertex(const Vec3<float>& FromPos, const Vec3<float>& ToPos, const Color& Color, const float& Thickness)
			:fromPos(FromPos), toPos(ToPos), color(Color), thickness(Thickness) {}
	};

	GenerateDrawLinePipeline(targetFormat, BlendMode);

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_drawLinePipeline[targetFormat][BlendMode]);

	if (LINE_VERTEX_BUFF.size() < (s_drawLineCount + 1))
	{
		LINE_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(LineVertex), 1, nullptr, ("DrawLine3D -" + std::to_string(s_drawLineCount)).c_str()));
	}

	LineVertex vertex(From, To, LineColor, Thickness);
	LINE_VERTEX_BUFF[s_drawLineCount]->Mapping(&vertex);
	Vec3<float>center = From.GetCenter(To);

	KuroEngine::Instance()->Graphics().ObjectRender(
		LINE_VERTEX_BUFF[s_drawLineCount],
		{
			{Cam.GetBuff(),CBV}
		},
		center.z,
		true);

	s_drawLineCount++;
}

void DrawFunc3D::DrawNonShadingModel(const std::weak_ptr<Model> Model, Transform& Transform, Camera& Cam, const float& Alpha, std::shared_ptr<ModelAnimator> Animator, const AlphaBlendMode& BlendMode)
{
	static std::map<DXGI_FORMAT, std::array<std::shared_ptr<GraphicsPipeline>, AlphaBlendModeNum>>PIPELINE;
	static std::vector<std::shared_ptr<ConstantBuffer>>DRWA_DATA_BUFF;

	struct DrawData
	{
		Matrix m_transformMat;
		float m_alpha = 1.0f;
	};

	const auto targetFormat = KuroEngine::Instance()->Graphics().GetRecentRenderTargetFormat(0);

	//�p�C�v���C��������
	if (!PIPELINE[targetFormat][BlendMode])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawNonShadingModel.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawNonShadingModel.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�`��f�[�^�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�{�[���s��o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�J���[�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�e���A����{���o�b�t�@"),
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(targetFormat, BlendMode) };
		//�p�C�v���C������
		PIPELINE[targetFormat][BlendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, {WrappedSampler(false, false)});
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[targetFormat][BlendMode]);

	if (DRWA_DATA_BUFF.size() < (s_drawNonShadingCount + 1))
	{
		DRWA_DATA_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(DrawData), 1, nullptr, ("DrawShadingModel_DrawData -" + std::to_string(s_drawNonShadingCount)).c_str()));
	}

	DrawData drawData;
	drawData.m_transformMat = Transform.GetMat();
	drawData.m_alpha = Alpha;
	DRWA_DATA_BUFF[s_drawNonShadingCount]->Mapping(&drawData);

	auto model = Model.lock();
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (Animator)boneBuff = Animator->GetBoneMatBuff();

	for (int meshIdx = 0; meshIdx < model->m_meshes.size(); ++meshIdx)
	{
		const auto& mesh = model->m_meshes[meshIdx];
		KuroEngine::Instance()->Graphics().ObjectRender(
			mesh.mesh->vertBuff,
			mesh.mesh->idxBuff,
			{
				{Cam.GetBuff(),CBV},
				{DRWA_DATA_BUFF[s_drawNonShadingCount],CBV},
				{boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->buff,CBV}
			},
			Transform.GetPos().z,
			true);
	}

	s_drawNonShadingCount++;
}

void DrawFunc3D::DrawADSShadingModel(LightManager& LigManager, const std::weak_ptr<Model> Model, Transform& Transform, Camera& Cam, std::shared_ptr<ModelAnimator> Animator, const AlphaBlendMode& BlendMode)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	static std::vector<std::shared_ptr<ConstantBuffer>>TRANSFORM_BUFF;

	//�p�C�v���C��������
	if (!PIPELINE[BlendMode])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawADSShadingModel.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawADSShadingModel.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�����X�t�H�[���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�{�[���s��o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�J���[�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�m�[�}���}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�e���A����{���o�b�t�@"),

		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), BlendMode) };
		//�p�C�v���C������
		PIPELINE[BlendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, {WrappedSampler(false, false)});
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[BlendMode]);

	if (TRANSFORM_BUFF.size() < (s_drawAdsShadingCount + 1))
	{
		TRANSFORM_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("DrawADSShadingModel_Transform -" + std::to_string(s_drawAdsShadingCount)).c_str()));
	}

	TRANSFORM_BUFF[s_drawAdsShadingCount]->Mapping(&Transform.GetMat());

	auto model = Model.lock();
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (Animator)boneBuff = Animator->GetBoneMatBuff();
	
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
				{TRANSFORM_BUFF[s_drawAdsShadingCount],CBV},
				{boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->texBuff[NORMAL_TEX],SRV},
				{mesh.material->buff,CBV}
			},
			Transform.GetPos().z,
			true);
	}

	s_drawAdsShadingCount++;
}

void DrawFunc3D::DrawPBRShadingModel(LightManager& LigManager, const std::weak_ptr<Model> Model, Transform& Transform, Camera& Cam, std::shared_ptr<ModelAnimator> Animator, std::shared_ptr<CubeMap>AttachCubeMap, const AlphaBlendMode& BlendMode)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	static std::vector<std::shared_ptr<ConstantBuffer>>TRANSFORM_BUFF;

	//�p�C�v���C��������
	if (!PIPELINE[BlendMode])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawPBRShadingModel.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawPBRShadingModel.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�L���[�u�}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�����X�t�H�[���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�{�[���s��o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�x�[�X�J���[�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"���^���l�X�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�m�[�}���}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�e���A����{���o�b�t�@"),

		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), BlendMode) };
		//�p�C�v���C������
		PIPELINE[BlendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, {WrappedSampler(false, false)});
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[BlendMode]);

	if (TRANSFORM_BUFF.size() < (s_drawPbrShadingCount + 1))
	{
		TRANSFORM_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("DrawPBRShadingModel_Transform -" + std::to_string(s_drawPbrShadingCount)).c_str()));
	}

	TRANSFORM_BUFF[s_drawPbrShadingCount]->Mapping(&Transform.GetMat());


	//�{�[���s��o�b�t�@�擾�i�A�j���[�^�[��nullptr�Ȃ��j
	auto model = Model.lock();
	std::shared_ptr<ConstantBuffer>boneBuff;
	if (Animator)boneBuff = Animator->GetBoneMatBuff();

	//�L���[�u�}�b�v�inullptr�Ȃ�f�t�H���g�̐ÓI�L���[�u�}�b�v�j
	std::shared_ptr<CubeMap>cubeMap = StaticallyCubeMap::GetDefaultCubeMap();
	if (AttachCubeMap)cubeMap = AttachCubeMap;

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
				{cubeMap->GetCubeMapTex(),SRV},
				{TRANSFORM_BUFF[s_drawPbrShadingCount],CBV},
				{boneBuff,CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->texBuff[METALNESS_TEX],SRV},
				{mesh.material->texBuff[NORMAL_TEX],SRV},
				{mesh.material->texBuff[ROUGHNESS_TEX],SRV},
				{mesh.material->buff,CBV},
			},
			Transform.GetPos().z,
			true);
	}

	s_drawPbrShadingCount++;
}

void DrawFunc3D::DrawToonModel(const std::weak_ptr<TextureBuffer> ToonTex, LightManager& LigManager, const std::weak_ptr<Model> Model, Transform& Transform, Camera& Cam, const AlphaBlendMode& BlendMode)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE[AlphaBlendModeNum];
	static std::vector<std::shared_ptr<ConstantBuffer>>TRANSFORM_BUFF;

	//�p�C�v���C��������
	if (!PIPELINE[BlendMode])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawToonModel.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawToonModel.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�g�����X�t�H�[���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�J���[�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�m�[�}���}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�g�D�[���e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�e���A����{���o�b�t�@"),

		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), BlendMode) };
		//�p�C�v���C������
		PIPELINE[BlendMode] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, {WrappedSampler(false, false)});
	}

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE[BlendMode]);

	if (TRANSFORM_BUFF.size() < (s_drawToonCount + 1))
	{
		TRANSFORM_BUFF.emplace_back(D3D12App::Instance()->GenerateConstantBuffer(sizeof(Matrix), 1, nullptr, ("DrawShadingModel_Transform -" + std::to_string(s_drawToonCount)).c_str()));
	}

	TRANSFORM_BUFF[s_drawToonCount]->Mapping(&Transform.GetMat());

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
				{TRANSFORM_BUFF[s_drawToonCount],CBV},
				{mesh.material->texBuff[COLOR_TEX],SRV},
				{mesh.material->texBuff[NORMAL_TEX],SRV},
				{ToonTex.lock(),SRV},
				{mesh.material->buff,CBV}
			},
			Transform.GetPos().z,
			true);
	}

	s_drawToonCount++;
}