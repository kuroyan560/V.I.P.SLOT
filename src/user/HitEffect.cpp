#include "HitEffect.h"
#include"KuroEngine.h"
#include"DrawFunc2D.h"
#include"NoiseGenerator.h"
#include"Camera.h"

std::array<HitEffect, HitEffect::MAX_NUM>HitEffect::s_instanceArray;
std::shared_ptr<HitParticle> HitEffect::s_particle;
static const Vec2<int>IMG_SIZE = { 512,512 };

void HitEffect::Generate(const Vec3<float>& Pos)
{
	for (auto& e : s_instanceArray)
	{
		if (e.m_isActive)continue;

		e.m_isActive = 1;
		e.m_pos = Pos;
		e.m_scale = 0.0f;
		e.m_rotate = KuroFunc::GetRand(Angle::ROUND());
		e.m_alpha = 1.0f;
		e.m_lifeTimer = 0.0f;
		e.m_lifeSpan = 15;
		e.m_blur = 34.158f;
		e.m_uvRadiusOffset = 0.0f;
		e.m_circleThickness = 0.125f;
		e.m_circleRadius = 0.18f;
		break;
	}
	s_particle->Emit(50, Pos);
}

void HitEffect::Init(Camera& Cam)
{
	for (auto& e : s_instanceArray)e.m_isActive = 0;

	if (!s_particle)s_particle = std::make_shared<HitParticle>();
	s_particle->Init(Cam);
}

void HitEffect::Update()
{
	for (auto& e : s_instanceArray)
	{
		if (!e.m_isActive)continue;

		float rate = e.m_lifeTimer;
		//e.circleRadius = KuroMath::Ease(In, Circ, rate, 0.192f, 0.186f);
		e.m_circleThickness = KuroMath::Ease(Out, Exp, rate, 0.125f, 0.212f);
		e.m_uvRadiusOffset = KuroMath::Ease(Out, Exp, rate, 0.0f, 0.05f);
		e.m_scale = KuroMath::Ease(Out, Exp, rate, 0.0f, 1.0f);
		e.m_alpha = KuroMath::Ease(In, Circ, rate, 1.0f, 0.0f);

		//����
		e.m_lifeTimer += 1.0f / e.m_lifeSpan;
		if (1.0f <= e.m_lifeTimer)e.m_isActive = false;
	}

	s_particle->Update();
}

void HitEffect::Draw(Camera& Cam)
{
	static std::shared_ptr<GraphicsPipeline>PIPELINE;
	static std::shared_ptr<TextureBuffer>DISPLACEMENT_NOISE_TEX;
	static std::shared_ptr<TextureBuffer>ALPHA_NOISE_TEX;
	static std::shared_ptr<VertexBuffer>VERTEX_BUFF;
	if (!PIPELINE)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.m_depthWriteMask = false;
		PIPELINE_OPTION.m_depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/user/shaders/HitEffect.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_gs = D3D12App::Instance()->CompileShader("resource/user/shaders/HitEffect.hlsl", "GSmain", "gs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/user/shaders/HitEffect.hlsl", "PSmain", "ps_6_4");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("ALIVE",DXGI_FORMAT_R32_SINT),
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32B32_FLOAT),
			InputLayoutParam("SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ROTATE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ALPHA",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIFE_TIMER",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIFE_SPAN",DXGI_FORMAT_R32_SINT),
			InputLayoutParam("BLUR",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("UV_RADIUS_OFFSET",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("CIRCLE_THICKNESS",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("CIRCLE_RADIUS",DXGI_FORMAT_R32_FLOAT),
		};

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�����萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�f�B�X�v���C�X�����g�p�p�[�����m�C�Y�e�N�X�`��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�A���t�@�p�p�[�����m�C�Y�e�N�X�`��"),
		};

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_Add) };
		//�p�C�v���C������
		PIPELINE = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(true, true) });

		//�p�[�����m�C�Y�i�f�B�X�v���C�X�����g�}�b�v�j
		NoiseInitializer displacementNoiseInit;
		displacementNoiseInit.m_split = { 12,12 };
		displacementNoiseInit.m_contrast = 1;
		displacementNoiseInit.m_octave = 6;
		displacementNoiseInit.m_frequency = 1.647f;
		displacementNoiseInit.m_persistance = 0.775f;
		DISPLACEMENT_NOISE_TEX = NoiseGenerator::PerlinNoise2D("HitEffect - DisplacementNoiseTex", IMG_SIZE, displacementNoiseInit);

		//�p�[�����m�C�Y�i�A���t�@�j
		NoiseInitializer alphaNoiseInit;
		alphaNoiseInit.m_split = { 7,7 };
		alphaNoiseInit.m_contrast = 2;
		alphaNoiseInit.m_octave = 2;
		alphaNoiseInit.m_frequency = 0.79f;
		alphaNoiseInit.m_persistance = 0.5f;
		ALPHA_NOISE_TEX = NoiseGenerator::PerlinNoise2D("HitEffect - AlphaNoiseTex", IMG_SIZE, alphaNoiseInit);

		//���_�o�b�t�@����
		VERTEX_BUFF = D3D12App::Instance()->GenerateVertexBuffer(sizeof(HitEffect), MAX_NUM, nullptr, "HitEffect - VertexBuffer");
	}

	//���_�o�b�t�@�f�[�^�]��
	VERTEX_BUFF->Mapping(s_instanceArray.data());

	//�p�C�v���C���Z�b�g
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(PIPELINE);

	KuroEngine::Instance()->Graphics().ObjectRender(
		VERTEX_BUFF,
		{
			{Cam.GetBuff(),CBV},
			{DISPLACEMENT_NOISE_TEX,SRV},
			{ALPHA_NOISE_TEX,SRV}
		},
		0.0f, true
	);

	s_particle->Draw(Cam);
}
