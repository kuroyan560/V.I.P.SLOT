#include "DOF.h"
#include"KuroEngine.h"
#include"GaussianBlur.h"
#include"DrawFunc2D.h"

std::shared_ptr<ComputePipeline>DOF::s_pipeline;

void DOF::GeneratePipeline()
{
	std::vector<RootParam>rootParam =
	{
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"DOF�ݒ�"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�u���[���������e�N�X�`��"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�[�x�}�b�v"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"�`�����ݐ�o�b�t�@")
	};

	auto cs = D3D12App::Instance()->CompileShader("resource/engine/DOF.hlsl", "CSmain", "cs_6_4");
	s_pipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParam, { WrappedSampler(false, true) });
}

DOF::DOF()
{
	//�p�C�v���C���������Ȃ琶��
	if (!s_pipeline)GeneratePipeline();

	//�E�B���h�E�T�C�Y�̎擾
	const auto winSize = WinApp::Instance()->GetExpandWinSize();

	//�o�b�N�o�b�t�@�̎擾
	auto backBuffFormat = D3D12App::Instance()->GetBackBuffFormat();

	//DOF�ݒ�
	m_configBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(DOFConfig), 1, &m_config, "DOF - Config - ConstantBuffer");

	//�K�E�V�A���u���[����
	m_gaussianBlur = std::make_shared<GaussianBlur>(winSize.Int(), backBuffFormat);

	//�[�x�}�b�v�����Ƃɐ����������߃{�P�摜�̊i�[��
	m_processedTex = D3D12App::Instance()->GenerateTextureBuffer(winSize.Int(), backBuffFormat, "DOF - ProcessedTex");
}

void DOF::SetPintConfig(float FrontPint, float BackPint, float PintLength)
{
	m_config.m_nearPint = FrontPint;
	m_config.m_farPint = BackPint;
	m_config.m_pintLength = PintLength;
	m_configBuff->Mapping(&m_config);
}

void DOF::Draw(std::weak_ptr<RenderTarget> Src, std::weak_ptr<RenderTarget> DepthMap)
{
	m_gaussianBlur->Register(Src.lock());

	KuroEngine::Instance()->Graphics().SetComputePipeline(s_pipeline);

	static const int DIV = 32;
	Vec3<int>threadNum = 
	{ 
		static_cast<int>(ceil(m_processedTex->GetGraphSize().x / DIV)),
		static_cast<int>(ceil(m_processedTex->GetGraphSize().y / DIV)),
		1 
	};

	KuroEngine::Instance()->Graphics().Dispatch(threadNum,
		{
			{m_configBuff,CBV},
			{m_gaussianBlur->GetResultTex(),SRV},
			{DepthMap.lock(),SRV},
			{m_processedTex,UAV}
		});

	DrawFunc2D::DrawGraph({ 0,0 }, m_processedTex);
}
