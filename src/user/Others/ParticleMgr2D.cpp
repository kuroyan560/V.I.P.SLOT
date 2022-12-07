#include "ParticleMgr2D.h"
#include"KuroEngine.h"

ParticleMgr2D::ParticleMgr2D()
{
	//DirectX�@�\�擾
	auto d3d12app = D3D12App::Instance();

	//�S�p�[�e�B�N�����ʂ̒萔�o�b�t�@�p��
	m_commonConstBuff = d3d12app->GenerateConstantBuffer(
		sizeof(CommonConstData),
		1,
		&m_commonConstData,
		"ParticleMgr2D - CommonConstantBuffer");

	//�R���s���[�g�V�F�[�_�[�̃��[�g�p�����[�^�i���ʁj
	static const std::vector<RootParam>ROOT_PARAMS =
	{
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�^�C���X�P�[��"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"�p�[�e�B�N���z��"),
	};

	//�S�p�[�e�B�N���̍X�V���s���R���s���[�g�p�C�v���C��
	m_commonComputePipeline = d3d12app->GenerateComputePipeline(
		d3d12app->CompileShader("resource/user/shaders/Particle/ParticleMgr_Compute.hlsl", "CSmain", "cs_6_4"),
		ROOT_PARAMS,
		{ WrappedSampler(false, false) });


	//�`��p�O���t�B�b�N�p�C�v���C��
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.m_depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = d3d12app->CompileShader("resource/user/shaders/Particle/ParticleMgr_Graphics.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_gs = d3d12app->CompileShader("resource/user/shaders/Particle/ParticleMgr_Graphics.hlsl", "GSmain", "gs_6_4");
		SHADERS.m_ps = d3d12app->CompileShader("resource/user/shaders/Particle/ParticleMgr_Graphics.hlsl", "PSmain", "ps_6_4");

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			//���������
			InputLayoutParam("EMIT_MUL_COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
			InputLayoutParam("EMIT_POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EMIT_VEC",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EMIT_SPEED",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIFE_SPAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_TEX_IDX",DXGI_FORMAT_R32_UINT),

			//�p�����[�^
			InputLayoutParam("MUL_COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("SPEED",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIFE_TIMER",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ALIVE",DXGI_FORMAT_R16_UINT),
			InputLayoutParam("TEX_IDX",DXGI_FORMAT_R32_UINT),
		};

		//���[�g�p�����[�^
		//���s���e�s��萔�o�b�t�@
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
		};
		//�e�N�X�`���z��
		for (int i = 0; i < TEX_NUM_MAX; ++i)
		{
			ROOT_PARAMETER.emplace_back(RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, ("�e�N�X�`����� - " + std::to_string(i)).c_str()));
		}

		//�����_�[�^�[�Q�b�g�`�����
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(d3d12app->GetBackBuffFormat(),AlphaBlendMode_Trans),
		};

		//�p�C�v���C������
		m_graphicsPipeline = d3d12app->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });
	}
}

void ParticleMgr2D::Init()
{
	EraseAllParticles();
}

void ParticleMgr2D::Update(float arg_timeScale)
{
	//�S�p�[�e�B�N�����ʂ̒萔�o�b�t�@�X�V
	if (m_commonConstData.m_timeScale != arg_timeScale)
	{
		m_commonConstData.m_timeScale = arg_timeScale;
		m_commonConstBuff->Mapping(&m_commonConstData);
	}

	

	//�p�[�e�B�N����ʂ��ƂɃR���s���[�g�V�F�[�_�[�ōX�V
	for (auto& info : m_particleInfoArray)
	{
		//�R���s���[�g�p�C�v���C���ɃZ�b�g����f�B�X�N���v�^���
		std::vector<RegisterDescriptorData>descDatas =
		{
			{m_commonConstBuff,CBV},
			{info.m_vertBuff->GetRWStructuredBuff().lock(),UAV}
		};

		D3D12App::Instance()->DispathOneShot(
			info.m_cPipeline,
			Vec3<int>(static_cast<int>(info.m_particles.size()) / THREAD_PER_NUM + 1, 1, 1),
			descDatas);
	}

	//�S�p�[�e�B�N�����ʂ̏���
	for (auto& info : m_particleInfoArray)
	{
		//�R���s���[�g�p�C�v���C���ɃZ�b�g����f�B�X�N���v�^���
		std::vector<RegisterDescriptorData>descDatas =
		{
			{m_commonConstBuff,CBV},
			{info.m_vertBuff->GetRWStructuredBuff().lock(),UAV}
		};
		D3D12App::Instance()->DispathOneShot(
			m_commonComputePipeline,
			Vec3<int>(static_cast<int>(info.m_particles.size()) / THREAD_PER_NUM + 1, 1, 1),
			descDatas);
	}


}

#include"LightManager.h"
void ParticleMgr2D::Draw()
{
	//�O���t�B�b�N�X�����擾
	auto& graphics = KuroEngine::Instance()->Graphics();

	//�p�[�e�B�N����ʂ��Ƃɕ`��
	for (auto& info : m_particleInfoArray)
	{
		//���s���e�s��o�b�t�@
		std::vector<RegisterDescriptorData>descDatas =
		{
			{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
		};

		//�e�N�X�`���z��
		for (auto& tex : info.m_textures)
		{
			descDatas.push_back({ tex ,SRV });
		}

		graphics.SetGraphicsPipeline(m_graphicsPipeline);
		graphics.ObjectRender(
			info.m_vertBuff,
			descDatas,
			1.0f,
			true,
			1);
	}
}

int ParticleMgr2D::Prepare(int arg_maxInstanceNum, std::string arg_computeShaderPath, std::shared_ptr<TextureBuffer>* arg_texArray, size_t arg_texArraySize, ParticleInitializer* arg_defaultInitializer)
{
	//�Z�b�g�ł���e�N�X�`���̍ő吔�𒴂��Ă���
	assert(static_cast<int>(arg_texArraySize) < TEX_NUM_MAX);
	
	//�z��̃C���f�b�N�X���p�[�e�B�N��ID�Ƃ���
	int particleID = static_cast<int>(m_particleInfoArray.size());

	//�p�[�e�B�N�����ǉ�
	m_particleInfoArray.emplace_back();

	//�Q�Ǝ擾
	auto& info = m_particleInfoArray.back();
	//DirectX�@�\�擾
	auto d3d12App = D3D12App::Instance();

	//�ő�C���X�^���X�����p�[�e�B�N���p��
	info.m_particles.resize(arg_maxInstanceNum);

	//�p�[�e�B�N�����𒸓_�o�b�t�@�Ƃ��Đ����A���M
	info.m_vertBuff = d3d12App->GenerateVertexBuffer(
		sizeof(Particle),
		arg_maxInstanceNum,
		info.m_particles.data(),
		("ParticleMgr - VertexBuffer - " + std::to_string(particleID)).c_str(),
		true);

	//�R���s���[�g�V�F�[�_�[�̃��[�g�p�����[�^�i���ʁj
	static const std::vector<RootParam>ROOT_PARAMS =
	{
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�^�C���X�P�[��"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"�p�[�e�B�N���z��"),
	};
	//�R���s���[�g�V�F�[�_�[����
	info.m_cPipeline = d3d12App->GenerateComputePipeline(
		d3d12App->CompileShader(arg_computeShaderPath, "CSmain", "cs_6_4"),
		ROOT_PARAMS,
		{ WrappedSampler(false, false) });

	//�e�N�X�`���z����L�^
	for (int i = 0; i < static_cast<int>(arg_texArraySize); ++i)
	{
		info.m_textures.emplace_back(arg_texArray[i]);
	}

	//�f�t�H���g�̏�������񂪂���΋L�^
	if (arg_defaultInitializer != nullptr)
	{
		info.m_defaultInitializer = new ParticleInitializer(*arg_defaultInitializer);
	}

	return particleID;
}

void ParticleMgr2D::Emit(int arg_particleID, ParticleInitializer arg_initializer)
{
	auto& info = m_particleInfoArray[arg_particleID];

	//�p�[�e�B�N�����o
	info.m_particles[info.m_nextEmitParticleIdx].Generate(arg_initializer);
	//GPU�ɑ��M
	info.m_vertBuff->Mapping(&info.m_particles[info.m_nextEmitParticleIdx], 1, info.m_nextEmitParticleIdx);

	//���ɕ��o����p�[�e�B�N���̃C���f�b�N�X���C���N�������g�A�Ō�̃p�[�e�B�N���܂ł�����0�Ԗڂɖ߂�
	if (static_cast<int>(info.m_particles.size()) <= ++info.m_nextEmitParticleIdx)info.m_nextEmitParticleIdx = 0;
}

void ParticleMgr2D::EraseParticles(int arg_particleID)
{
	auto& info = m_particleInfoArray[arg_particleID];

	//���ɕ��o����p�[�e�B�N���̃C���f�b�N�X��������
	info.m_nextEmitParticleIdx = 0;

	//�S�Ẵp�[�e�B�N���̐����t���O��OFF��
	for (auto& particle : info.m_particles)
	{
		particle.m_isAlive = 0;
	}

	//�o�b�t�@���M
	info.m_vertBuff->Mapping(info.m_particles.data());
}
