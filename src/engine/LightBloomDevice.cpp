#include "LightBloomDevice.h"
#include"KuroEngine.h"
#include"GaussianBlur.h"
#include"DrawFunc2D.h"

std::shared_ptr<ComputePipeline>LightBloomDevice::s_filterPipeline;
std::shared_ptr<ComputePipeline>LightBloomDevice::s_combinePipeline;

void LightBloomDevice::GeneratePipeline()
{
	{
		//�V�F�[�_�[
		auto cs = D3D12App::Instance()->CompileShader("resource/engine/LightBloom.hlsl", "Filter", "cs_6_4");
		//���[�g�p�����[�^
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���C�g�u���[���ݒ�"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�G�~�b�V�u�}�b�v"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"���H��G�~�b�V�u�}�b�v"),
		};
		//�p�C�v���C������
		s_filterPipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams, { WrappedSampler(false,true) });
	}
	{
		//�V�F�[�_�[
		auto cs = D3D12App::Instance()->CompileShader("resource/engine/LightBloom.hlsl", "Combine", "cs_6_4");
		//���[�g�p�����[�^
		std::vector<RootParam>rootParams;
		for (int i = 0; i < BLUR_COUNT; ++i)
			rootParams.emplace_back(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�{�P�摜");
		rootParams.emplace_back(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, "���H��G�~�b�V�u�}�b�v");
		rootParams.emplace_back(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, "���C�g�u���[���ݒ�"),

		//�p�C�v���C������
		s_combinePipeline = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams, { WrappedSampler(false,true) });
	}
}

LightBloomDevice::LightBloomDevice()
{
	//�p�C�v���C������
	if (!s_filterPipeline)GeneratePipeline();

	//�E�B���h�E�T�C�Y�擾
	const auto winSize = WinApp::Instance()->GetExpandWinSize().Int();

	//�G�~�b�V�u�}�b�v
	m_processedEmissiveMap = D3D12App::Instance()->GenerateTextureBuffer(winSize, DXGI_FORMAT_R32G32B32A32_FLOAT, "LightBloom - EmissiveMap - Processed");

	//�萔�o�b�t�@����
	m_config.m_imgSize = winSize;
	m_constBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(LightBloomConfig), 1, &m_config, "LgihtBloom - Config - ConstantBuffer");

	//�K�E�V�A���u���[
	for (int i = 0; i < BLUR_COUNT; ++i)
		m_gaussianBlurs[i] = std::make_shared<GaussianBlur>(winSize, DXGI_FORMAT_R32G32B32A32_FLOAT);
}

void LightBloomDevice::Draw(std::weak_ptr<RenderTarget>EmissiveMap, std::weak_ptr<RenderTarget>Target)
{
	auto emissiveMap = EmissiveMap.lock();

	//�G�~�b�V�u�}�b�v���������l�Ȃǂɉ����ĉ��H
	KuroEngine::Instance()->Graphics().SetComputePipeline(s_filterPipeline);
	Vec3<int>threadNum = { emissiveMap->GetGraphSize().x / THREAD_DIV + 1,emissiveMap->GetGraphSize().y / THREAD_DIV + 1,1 };

	KuroEngine::Instance()->Graphics().Dispatch(threadNum,
		{
			{m_constBuff,CBV},
			{emissiveMap,SRV,D3D12_RESOURCE_STATE_COMMON},
			{m_processedEmissiveMap,UAV}
		});

	//�G�~�b�V�u�}�b�v�Ƀu���[��������
	m_gaussianBlurs[0]->Register(m_processedEmissiveMap);
	for (int i = 1; i < BLUR_COUNT; ++i)m_gaussianBlurs[i]->Register(m_gaussianBlurs[i - 1]->GetResultTex());

	//�u���[����
	KuroEngine::Instance()->Graphics().SetComputePipeline(s_combinePipeline);
	std::vector<RegisterDescriptorData>descDatas;
	for (int i = 0; i < BLUR_COUNT; ++i)
		descDatas.emplace_back(m_gaussianBlurs[i]->GetResultTex(), SRV, D3D12_RESOURCE_STATE_COMMON);
	descDatas.emplace_back(m_processedEmissiveMap, UAV);
	KuroEngine::Instance()->Graphics().Dispatch(threadNum, descDatas);

	//���ʂ�`��
	KuroEngine::Instance()->Graphics().SetRenderTargets({ Target.lock() });
	DrawFunc2D::DrawGraph({ 0,0 }, m_processedEmissiveMap, 1.0f, AlphaBlendMode_Add);
}

void LightBloomDevice::SetOutputColorRate(const Vec3<float>& RGBrate)
{
	m_config.m_outputColorRate = RGBrate;
	m_constBuff->Mapping(&m_config);
}

void LightBloomDevice::SetBrightThreshold(const float& Threshold)
{
	m_config.m_brightThreshold = Threshold;
	m_constBuff->Mapping(&m_config);
}

void LightBloomDevice::SetBlurPower(const float& BlurPower)
{
	for (int i = 0; i < BLUR_COUNT; ++i)
		m_gaussianBlurs[i]->SetBlurPower(BlurPower);
}
