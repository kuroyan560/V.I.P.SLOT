#include "Sprite.h"
#include"KuroEngine.h"

std::shared_ptr<GraphicsPipeline>Sprite::s_pipeline[AlphaBlendModeNum];
std::shared_ptr<TextureBuffer>Sprite::s_defaultTex;

Sprite::Sprite(const std::shared_ptr<TextureBuffer>& Texture, const char* Name) : m_mesh(Name)
{
	if (!s_pipeline[0])
	{
		//�p�C�v���C���ݒ�
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		PIPELINE_OPTION.m_depthTest = false;

		//�V�F�[�_�[���
		static Shaders SHADERS;
		SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/engine/Sprite.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/engine/Sprite.hlsl", "PSmain", "ps_6_4");

		//���[�g�p�����[�^
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"���s���e�s��萔�o�b�t�@"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e�N�X�`���V�F�[�_�[���\�[�X"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J���[ & �s��萔�o�b�t�@")
		};

		//�����_�[�^�[�Q�b�g�`�����
		for (int i = 0; i < AlphaBlendModeNum; ++i)
		{
			std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), (AlphaBlendMode)i) };
			//�p�C�v���C������
			s_pipeline[i] = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, SpriteMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(true, false) });
		}

		//���e�N�X�`��
		s_defaultTex = D3D12App::Instance()->GenerateTextureBuffer(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	//�f�t�H���g�̃e�N�X�`���o�b�t�@
	m_texBuff = s_defaultTex;

	//�e�N�X�`���Z�b�g
	SetTexture(Texture);

	//�s�񏉊���
	m_constData.m_mat = m_transform.GetMat();

	//�萔�o�b�t�@����
	m_constBuff = D3D12App::Instance()->GenerateConstantBuffer(sizeof(m_constData), 1, &m_constData, Name);
}

void Sprite::SetTexture(const std::shared_ptr<TextureBuffer>& Texture)
{
	if (Texture == nullptr)return;
	m_texBuff = Texture;
}

void Sprite::SetColor(const Color& Color)
{
	if (m_constData.m_color == Color)return;
	m_constData.m_color = Color;
	m_constBuff->Mapping(&m_constData);
}

void Sprite::Draw(const AlphaBlendMode& BlendMode)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(s_pipeline[(int)BlendMode]);

	if (m_transform.GetDirty())
	{
		m_constData.m_mat = m_transform.GetMat();
		m_constBuff->Mapping(&m_constData);
	}

	m_mesh.Render({
		{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},	//���s���e�s��
		{m_texBuff,SRV},			//�e�N�X�`�����\�[�X
		{m_constBuff,CBV}		//�J���[ & ���[���h�s��
		});
}