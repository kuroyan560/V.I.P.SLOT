#pragma once
#include<memory>
class GraphicsPipeline;
class ConstantBuffer;

#include"Color.h"
#include"KuroMath.h"
#include"SpriteMesh.h"
#include"Transform2D.h"


class Sprite
{
private:
	static std::shared_ptr<GraphicsPipeline>s_pipeline[AlphaBlendModeNum];
	//���̃x�^�h��e�N�X�`��
	static std::shared_ptr<TextureBuffer>s_defaultTex;

private:
	//�萔�o�b�t�@���M�p�f�[�^
	struct ConstantData
	{
		Matrix m_mat;
		Color m_color;
	}m_constData;

	//�萔�o�b�t�@
	std::shared_ptr<ConstantBuffer>m_constBuff;

	//�e�N�X�`���o�b�t�@
	std::shared_ptr<TextureBuffer>m_texBuff;

public:
	//�g�����X�t�H�[��
	Transform2D m_transform;
	//���b�V���i���_���j
	SpriteMesh m_mesh;

	//�e�N�X�`���A�X�v���C�g��
	Sprite(const std::shared_ptr<TextureBuffer>& Texture = nullptr, const char* Name = nullptr);

	//�e�N�X�`���Z�b�g
	void SetTexture(const std::shared_ptr<TextureBuffer>& Texture);

	//�F�Z�b�g�i�`��̍ۂɂ��̒l����Z�����j
	void SetColor(const Color& Color);
	
	//�`��
	void Draw(const AlphaBlendMode& BlendMode = AlphaBlendMode_Trans);
};