#include "HitParticle.h"
#include"D3D12App.h"
#include"Camera.h"
#include"KuroEngine.h"

static const float COL_MIN = 0.5f;
static const float COL_MAX = 0.9f;
static const UINT COMPUTE_THREAD_BLOCK_SIZE = 128;

HitParticle::HitParticle()
{
	//�u���b�N�̌̏�񐶐�
	m_particleBuff = D3D12App::Instance()->GenerateRWStructuredBuffer(sizeof(Particle), s_particleNum, nullptr, "IndirectSample - BlockBuffer");

	//�O���t�B�b�N�X�p���[�g�p�����[�^
	std::vector<RootParam>gRootParams
	{
		RootParam(CBV,"�J�����萔�o�b�t�@"),
		RootParam(CBV,"�e�u���b�N�萔�o�b�t�@"),
	};

	//�O���t�B�b�N�X�p�C�v���C������
	{
		//�p�C�v���C���ݒ�
		PipelineInitializeOption gPipelineOption(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

		//�V�F�[�_�[���
		Shaders gPipelineShaders;
		gPipelineShaders.m_vs = D3D12App::Instance()->CompileShader("resource/user/shaders/HitParticle_Graphics.hlsl", "VSmain", "vs_6_4");
		gPipelineShaders.m_gs = D3D12App::Instance()->CompileShader("resource/user/shaders/HitParticle_Graphics.hlsl", "GSmain", "gs_6_4");
		gPipelineShaders.m_ps = D3D12App::Instance()->CompileShader("resource/user/shaders/HitParticle_Graphics.hlsl", "PSmain", "ps_6_4");

		//�����_�[�^�[�Q�b�g�`�����
		std::vector<RenderTargetInfo>renderTargetInfo = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None) };

		//�p�C�v���C������
		m_gPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(
			gPipelineOption, 
			gPipelineShaders,
			{ InputLayoutParam("POSITION",DXGI_FORMAT_R32G32B32_FLOAT) },
			gRootParams,
			renderTargetInfo, 
			{ WrappedSampler(false, false) });
	}

	//�X�V�p�R���s���[�g�p�C�v���C��
	{
		//�V�F�[�_�[
		auto cs = D3D12App::Instance()->CompileShader(
			"resource/user/shaders/HitParticle_Compute.hlsl", "CSmain", "cs_6_0");

		//���[�g�p�����[�^
		std::vector<RootParam>cRootParams
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�ݒ�"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"�e�u���b�N�̏��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�R�}���h�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"�J�����O��̃R�}���h�o�b�t�@"),
		};

		m_cUpdatePipeline = D3D12App::Instance()->GenerateComputePipeline(
			cs, cRootParams, { WrappedSampler(false,false) });
	}


	m_indirectDev = D3D12App::Instance()->GenerateIndirectDevice(EXCUTE_INDIRECT_TYPE::DRAW, gRootParams, { WrappedSampler(false,false) });

	Vec3<float>initPos = { 0,0,0 };
	m_vertBuff = D3D12App::Instance()->GenerateVertexBuffer(sizeof(Vec3<float>), 1, &initPos, "IndirectSample - VertexBuffer");

	m_configBuffer = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Config), 1, &m_config, "IndirectSample - CallingConfig");
}

void HitParticle::Init(Camera& Cam)
{
	//�p�[�e�B�N���̏�����
	std::array<Particle, s_particleNum>particleInitData;
	m_particleBuff->Mapping(particleInitData.data());

	if (m_invalidCommandBuffer)
	{
		//������
		std::array<IndirectDrawCommand<2>, s_particleNum>commands;
		D3D12_GPU_VIRTUAL_ADDRESS camBuffAddress = Cam.GetBuff()->GetResource()->GetBuff()->GetGPUVirtualAddress();
		D3D12_GPU_VIRTUAL_ADDRESS blockBuffAddress = m_particleBuff->GetResource()->GetBuff()->GetGPUVirtualAddress();
		auto incrementSize = sizeof(Particle);
		for (auto& com : commands)
		{
			com.m_drawArgs.VertexCountPerInstance = 1;
			com.m_drawArgs.InstanceCount = 1;
			com.m_drawArgs.StartInstanceLocation = 0;
			com.m_drawArgs.StartVertexLocation = 0;

			//CBV0�i�J�������j
			com.m_gpuAddressArray[0] = camBuffAddress;

			//CBV1�i�p�[�e�B�N�����j
			com.m_gpuAddressArray[1] = blockBuffAddress;
			blockBuffAddress += incrementSize;
		}

		//�R�}���h�o�b�t�@�쐬
		m_commandBuffer = D3D12App::Instance()->GenerateIndirectCommandBuffer(DRAW, s_particleNum, 2, false, commands.data(), "HitParticle - AllParticle");
		m_aliveCommandBuffer = D3D12App::Instance()->GenerateIndirectCommandBuffer(DRAW, s_particleNum, 2, true, nullptr, "HitParticle - AliveParticle");

		m_invalidCommandBuffer = false;
	}

	//�����p�[�e�B�N���R�}���h�̃J�E���^�[�o�b�t�@�̃��Z�b�g
	m_aliveCommandBuffer->ResetCounterBuffer();
}

void HitParticle::Update()
{
	//m_config.camAddress = Cam.GetBuff()->GetResource()->GetBuff()->GetGPUVirtualAddress();
	//m_configBuffer->Mapping(&m_config);

	//�J�E���^�o�b�t�@���Z�b�g
	m_aliveCommandBuffer->ResetCounterBuffer();

	auto threadX = static_cast<int>(ceil(s_particleNum / float(COMPUTE_THREAD_BLOCK_SIZE)));
	std::vector<RegisterDescriptorData>descDatas =
	{
		RegisterDescriptorData(m_configBuffer,CBV),
		RegisterDescriptorData(m_particleBuff,UAV),
		RegisterDescriptorData(m_commandBuffer->GetBuff(),SRV),
		RegisterDescriptorData(m_aliveCommandBuffer->GetBuff(),UAV),
	};
	D3D12App::Instance()->DispathOneShot(m_cUpdatePipeline, Vec3<int>(threadX, 1, 1), descDatas);
}

void HitParticle::Draw(Camera& Cam)
{
	//�O���t�B�b�N�X
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(m_gPipeline);

	KuroEngine::Instance()->Graphics().ExecuteIndirectDraw(m_vertBuff, m_aliveCommandBuffer, m_indirectDev);
}

void HitParticle::Emit(int Num, Vec3<float>Pos)
{
	//CPU��ł̃p�[�e�B�N���o�b�t�@�̃|�C���^�擾
	Particle* ptArray = m_particleBuff->GetResource()->GetBuffOnCpu<Particle>();

	int lifeSpan = KuroFunc::GetRand(45, 60);
	Color col;
	col.m_r = KuroFunc::GetRand(0.3f, 0.5f);
	col.m_g = KuroFunc::GetRand(0.3f, 0.5f);
	col.m_b = KuroFunc::GetRand(0.8f, 1.0f);

	int generateNum = 0;
	for (int ptIdx = 0; ptIdx < s_particleNum; ++ptIdx)
	{
		//�p�[�e�B�N���擾
		auto& pt = ptArray[ptIdx];

		//�ғ����Ȃ�X���[
		if (pt.m_life)continue;

		pt.m_pos = Pos;
		pt.m_vel = KuroFunc::GetRand(Vec3<float>(-1, -1, -1), Vec3<float>(1, 1, 1));
		pt.m_color = col;
		pt.m_life = 1.0f;
		pt.m_lifeSpan = lifeSpan;

		//�w�萔���A��������
		if (Num == ++generateNum)return;
	}

	//����Ȃ�����
	assert(0);
}
