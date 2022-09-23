#include "DrawFunc2D_Shadow.h"
#include"KuroEngine.h"
#include"LightManager.h"

std::shared_ptr<ConstantBuffer>DrawFunc2D_Shadow::s_eyePosBuff;
std::shared_ptr<TextureBuffer>DrawFunc2D_Shadow::s_defaultTex;
std::shared_ptr<TextureBuffer>DrawFunc2D_Shadow::s_defaultNormalMap;
std::shared_ptr<TextureBuffer>DrawFunc2D_Shadow::s_defaultEmissiveMap;

//DrawExtendGraph
int DrawFunc2D_Shadow::s_drawExtendGraphCount = 0;

//DrawRotaGraph
int DrawFunc2D_Shadow::s_drawRotaGraphCount = 0;

static std::vector<RootParam>ROOT_PARAMETER =
{
	RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "���s���e�s��萔�o�b�t�@"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "���_���W���"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "�A�N�e�B�u���̃��C�g���o�b�t�@"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�|�C���g���C�g��� (�\�����o�b�t�@)"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�V�����C�g��� (�\�����o�b�t�@)"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�e�N�X�`�����\�[�X"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�@���}�b�v"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "���˃}�b�v")
};

void DrawFunc2D_Shadow::StaticInit()
{
	if (s_eyePosBuff)return;

	//���_���W
	Vec3<float>INIT_EYE_POS = { WinApp::Instance()->GetExpandWinCenter().x,WinApp::Instance()->GetExpandWinCenter().y,-5.0f };
	s_eyePosBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Vec3<float>), 1, &INIT_EYE_POS, "DrawFunc_Shadow - EYE_POS");

	//���e�N�X�`��
	s_defaultTex = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f));

	// (0,0,-1) �m�[�}���}�b�v
	s_defaultNormalMap = D3D12App::Instance()->GenerateTextureBuffer(Color(0.5f, 0.5f, 1.0f, 1.0f));

	//���e�N�X�`��
	s_defaultEmissiveMap = D3D12App::Instance()->GenerateTextureBuffer(Color(0.0f, 0.0f, 0.0f, 1.0f));
}

void DrawFunc2D_Shadow::SetEyePos(Vec3<float> EyePos)
{
	s_eyePosBuff->Mapping(&EyePos);
}

void DrawFunc2D_Shadow::DrawExtendGraph2D(LightManager& LigManager, const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap, const float& SpriteDepth, const Vec2<bool>& Miror, const float& Diffuse, const float& Specular, const float& Lim)
{
	//DrawExtendGraph��p���_
	class ExtendGraphVertex
	{
	public:
		Vec2<float>leftUpPos;
		Vec2<float>rightBottomPos;
		Vec2<int> miror;
		float depth;
		float diffuse;
		float specular;
		float lim;
		ExtendGraphVertex(const Vec2<float>& LeftUpPos, const Vec2<float>& RightBottomPos, const Vec2<bool>& Miror,
			const float& Depth, const float& Diffuse, const float& Specular, const float& Lim)
			:leftUpPos(LeftUpPos), rightBottomPos(RightBottomPos), miror({ Miror.x ? 1 : 0 ,Miror.y ? 1 : 0 }),
			depth(Depth), diffuse(Diffuse), specular(Specular), lim(Lim) {}
	};

	static std::shared_ptr<GraphicsPipeline>EXTEND_GRAPH_PIPELINE;
	static std::vector<std::shared_ptr<VertexBuffer>>EXTEND_GRAPH_VERTEX_BUFF;

	//�p�C�v���C��������
	if (!EXTEND_GRAPH_PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.m_depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawExtendGraph_Shadow.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_gs = D3D12App::Instance()->CompileShader("resource/engine/DrawExtendGraph_Shadow.hlsl", "GSmain", "gs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawExtendGraph_Shadow.hlsl", "PSmain", "ps_6_4");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION_L_U",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("POSITION_R_B",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MIROR",DXGI_FORMAT_R32G32_SINT),
			InputLayoutParam("DEPTH",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("DIFFUSE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SPECULAR",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIM",DXGI_FORMAT_R32_FLOAT)
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//�G�~�b�V�u
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//�p�C�v���C������
		EXTEND_GRAPH_PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, true) });
	}

	StaticInit();

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(EXTEND_GRAPH_PIPELINE);

	if (EXTEND_GRAPH_VERTEX_BUFF.size() < (s_drawExtendGraphCount + 1))
	{
		EXTEND_GRAPH_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(ExtendGraphVertex), 1, nullptr, ("DrawExtendGraph_Shadow -" + std::to_string(s_drawExtendGraphCount)).c_str()));
	}

	ExtendGraphVertex vertex(LeftUpPos, RightBottomPos, Miror, SpriteDepth, Diffuse, Specular, Lim);
	EXTEND_GRAPH_VERTEX_BUFF[s_drawExtendGraphCount]->Mapping(&vertex);

	auto normalMap = NormalMap ? NormalMap : s_defaultNormalMap;
	auto emissiveMap = EmissiveMap ? EmissiveMap : s_defaultEmissiveMap;

	std::vector<RegisterDescriptorData>descDatas =
	{
		{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
		{s_eyePosBuff,CBV},	//���_���W
		{LigManager.GetLigNumInfo(),CBV},	//�A�N�e�B�u���̃��C�g��
		{LigManager.GetLigInfo(Light::DIRECTION),SRV},	//�f�B���N�V�������C�g
		{LigManager.GetLigInfo(Light::POINT),SRV},			//�|�C���g���C�g
		{LigManager.GetLigInfo(Light::SPOT),SRV},			//�X�|�b�g���C�g
		{LigManager.GetLigInfo(Light::HEMISPHERE),SRV},	//�V�����C�g
		{Tex,SRV},
		{normalMap,SRV},
		{emissiveMap,SRV}
	};

	KuroEngine::Instance()->Graphics().ObjectRender(EXTEND_GRAPH_VERTEX_BUFF[s_drawExtendGraphCount],
		descDatas, 0.0f, true);

	s_drawExtendGraphCount++;
}

void DrawFunc2D_Shadow::DrawRotaGraph2D(LightManager& LigManager, const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian, const std::shared_ptr<TextureBuffer>& Tex, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap, const float& SpriteDepth, const Vec2<float>& RotaCenterUV, const Vec2<bool>& Miror, const float& Diffuse, const float& Specular, const float& Lim)
{
	//DrawRotaGraph��p���_
	class RotaGraphVertex
	{
	public:
		Vec2<float>center;
		Vec2<float> extRate;
		float radian;
		Vec2<float>rotaCenterUV;
		Vec2<int> miror;
		float depth;
		float diffuse;
		float specular;
		float lim;
		RotaGraphVertex(const Vec2<float>& Center, const Vec2<float>& ExtRate, const float& Radian,
			const Vec2<float>& RotaCenterUV, const Vec2<bool>& Miror,
			const float& Depth, const float& Diffuse, const float& Specular, const float& Lim)
			:center(Center), extRate(ExtRate), radian(Radian), rotaCenterUV(RotaCenterUV), miror({ Miror.x ? 1 : 0,Miror.y ? 1 : 0 }),
			depth(Depth), diffuse(Diffuse), specular(Specular), lim(Lim) {}
	};

	static std::shared_ptr<GraphicsPipeline>ROTA_GRAPH_PIPELINE;
	static std::vector<std::shared_ptr<VertexBuffer>>ROTA_GRAPH_VERTEX_BUFF;

	//�p�C�v���C��������
	if (!ROTA_GRAPH_PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.m_depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/DrawRotaGraph_Shadow.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_gs = D3D12App::Instance()->CompileShader("resource/engine/DrawRotaGraph_Shadow.hlsl", "GSmain", "gs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/DrawRotaGraph_Shadow.hlsl", "PSmain", "ps_6_4");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("CENTER",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EXT_RATE",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ROTA_CENTER_UV",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("MIROR",DXGI_FORMAT_R32G32_SINT),
			InputLayoutParam("DEPTH",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("DIFFUSE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SPECULAR",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIM",DXGI_FORMAT_R32_FLOAT)
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//�G�~�b�V�u
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//�p�C�v���C������
		ROTA_GRAPH_PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false,true) });
	}

	StaticInit();

	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(ROTA_GRAPH_PIPELINE);

	if (ROTA_GRAPH_VERTEX_BUFF.size() < (s_drawRotaGraphCount + 1))
	{
		ROTA_GRAPH_VERTEX_BUFF.emplace_back(D3D12App::Instance()->GenerateVertexBuffer(sizeof(RotaGraphVertex), 1, nullptr, ("DrawRotaGraph_Shadow -" + std::to_string(s_drawRotaGraphCount)).c_str()));
	}

	RotaGraphVertex vertex(Center, ExtRate, Radian, RotaCenterUV, Miror, SpriteDepth, Diffuse, Specular, Lim);
	ROTA_GRAPH_VERTEX_BUFF[s_drawRotaGraphCount]->Mapping(&vertex);

	auto normalMap = NormalMap ? NormalMap : s_defaultNormalMap;
	auto emissiveMap = EmissiveMap ? EmissiveMap : s_defaultEmissiveMap;

	KuroEngine::Instance()->Graphics().ObjectRender(ROTA_GRAPH_VERTEX_BUFF[s_drawRotaGraphCount],
		{
			{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
			{s_eyePosBuff,CBV},	//���_���W
			{LigManager.GetLigNumInfo(),CBV},	//�A�N�e�B�u���̃��C�g��
			{LigManager.GetLigInfo(Light::DIRECTION),SRV},	//�f�B���N�V�������C�g
			{LigManager.GetLigInfo(Light::POINT),SRV},	//�|�C���g���C�g
			{LigManager.GetLigInfo(Light::SPOT),SRV},	//�X�|�b�g���C�g
			{LigManager.GetLigInfo(Light::HEMISPHERE),SRV},	//�V�����C�g
			{Tex,SRV},
			{normalMap,SRV},
			{emissiveMap,SRV}
		},
		0.0f, true);

	s_drawRotaGraphCount++;
}