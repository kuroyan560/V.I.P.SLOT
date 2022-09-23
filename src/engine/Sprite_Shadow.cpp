#include "Sprite_Shadow.h"
#include"KuroEngine.h"
#include"LightManager.h"

std::shared_ptr<GraphicsPipeline>Sprite_Shadow::s_transPipeline;
std::shared_ptr<ConstantBuffer>Sprite_Shadow::s_EyePosBuff;
std::shared_ptr<TextureBuffer>Sprite_Shadow::s_defaultTex;
std::shared_ptr<TextureBuffer>Sprite_Shadow::s_defaultNormalMap;
std::shared_ptr<TextureBuffer>Sprite_Shadow::s_defaultEmissiveMap;

void Sprite_Shadow::SetEyePos(Vec3<float> EyePos)
{
	s_EyePosBuff->Mapping(&EyePos);
}

Sprite_Shadow::Sprite_Shadow(const std::shared_ptr<TextureBuffer>& Texture, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap, const char* Name)
{
	//�ÓI�����o�̏�����
	if (!s_transPipeline)
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.m_depthTest = false;

		//�C���v�b�g���C�A�E�g
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("TEXCOORD",DXGI_FORMAT_R32G32_FLOAT)
		};

		//�����_�[�^�[�Q�b�g�`�����
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//�o�b�N�o�b�t�@�̃t�H�[�}�b�g�A�A���t�@�u�����h
			RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(),AlphaBlendMode_Trans),
			//�G�~�b�V�u
			RenderTargetInfo(DXGI_FORMAT_R32G32B32A32_FLOAT,AlphaBlendMode_Trans)
		};

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/Sprite_Shadow.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/Sprite_Shadow.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J���[ & �s��萔 & �X�v���C�g��Z�ݒ�l�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���_���W���"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�A�N�e�B�u���̃��C�g���o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�f�B���N�V�������C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�|�C���g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�X�|�b�g���C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�V�����C�g��� (�\�����o�b�t�@)"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`���V�F�[�_�[���\�[�X"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�m�[�}���}�b�v�V�F�[�_�[���\�[�X"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�G�~�b�V�u�}�b�v�V�F�[�_�[���\�[�X"),
		};

		//�p�C�v���C������
		s_transPipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, true) });

		//���_���W
		Vec3<float>INIT_EYE_POS = { WinApp::Instance()->GetExpandWinCenter().x,WinApp::Instance()->GetExpandWinCenter().y,-5.0f };
		s_EyePosBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(Vec3<float>), 1, &INIT_EYE_POS, "Sprite_Shadow - EYE_POS");

		//���e�N�X�`��
		s_defaultTex = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f));

		// (0,0,-1) �m�[�}���}�b�v
		s_defaultNormalMap = D3D12App::Instance()->GenerateTextureBuffer(Color(0.5f, 0.5f, 1.0f, 1.0f));

		//���e�N�X�`��
		s_defaultEmissiveMap = D3D12App::Instance()->GenerateTextureBuffer(Color(0.0f, 0.0f, 0.0f, 1.0f));
	}

	//�f�t�H���g�̃e�N�X�`���o�b�t�@
	m_texBuff = s_defaultTex;
	m_normalMap = s_defaultNormalMap;
	m_emissiveMap = s_defaultEmissiveMap;
	
	//�e�N�X�`���Z�b�g
	SetTexture(Texture, NormalMap, EmissiveMap);

	//�s�񏉊���
	m_constData.m_mat = m_transform.GetMat();

	//�萔�o�b�t�@����
	m_constBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(m_constData), 1, &m_constData, Name);
}

void Sprite_Shadow::SetTexture(const std::shared_ptr<TextureBuffer>& Texture, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap)
{
	if (Texture != nullptr)
	{
		m_texBuff = Texture;
	}
	if (NormalMap != nullptr)
	{
		m_normalMap = NormalMap;
	}
	if (EmissiveMap != nullptr)
	{
		m_emissiveMap = EmissiveMap;
	}
}

void Sprite_Shadow::SetColor(const Color& Color)
{
	if (m_constData.m_color == Color)return;
	m_constData.m_color = Color;
	m_constBuff->Mapping(&m_constData);
}

void Sprite_Shadow::SetDepth(const float& Depth)
{
	if (m_constData.z == Depth)return;
	m_constData.z = Depth;
	m_constBuff->Mapping(&m_constData);
}

void Sprite_Shadow::SetDiffuseAffect(const float& Diffuse)
{
	if (m_constData.m_diffuse == Diffuse)return;
	m_constData.m_diffuse = Diffuse;
	m_constBuff->Mapping(&m_constData);
}

void Sprite_Shadow::SetSpecularAffect(const float& Specular)
{
	if (m_constData.m_specular == Specular)return;
	m_constData.m_specular = Specular;
	m_constBuff->Mapping(&m_constData);
}

void Sprite_Shadow::SetLimAffect(const float& Lim)
{
	if (m_constData.m_lim == Lim)return;
	m_constData.m_lim = Lim;
	m_constBuff->Mapping(&m_constData);
}

void Sprite_Shadow::Draw(LightManager& LigManager)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_transPipeline);

	if (m_transform.GetDirty())
	{
		m_constData.m_mat = m_transform.GetMat();
		m_constBuff->Mapping(&m_constData);
	}

	m_mesh.Render({
		{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
		{m_constBuff,CBV},
		{s_EyePosBuff,CBV},
		{LigManager.GetLigNumInfo(),CBV},
		{LigManager.GetLigInfo(Light::DIRECTION),SRV},
		{LigManager.GetLigInfo(Light::POINT),SRV},
		{LigManager.GetLigInfo(Light::SPOT),SRV},
		{LigManager.GetLigInfo(Light::HEMISPHERE),SRV},
		{m_texBuff,SRV},
		{m_normalMap,SRV},
		{m_emissiveMap,SRV},
		});
}
