#pragma once
#include<memory>
class GraphicsPipeline;
class ConstantBuffer;
class LightManager;
class TextureBuffer;

#include"Color.h"
#include"KuroMath.h"
#include"SpriteMesh.h"
#include"Transform2D.h"

class Sprite_Shadow
{
private:
	static std::shared_ptr<GraphicsPipeline>s_transPipeline;
	//���_���W�p�̒萔�o�b�t�@
	static std::shared_ptr<ConstantBuffer>s_EyePosBuff;
	//���̃x�^�h��e�N�X�`��
	static std::shared_ptr<TextureBuffer>s_defaultTex;
	// (0,0,-1) �̃x�^�h��m�[�}���}�b�v
	static std::shared_ptr<TextureBuffer>s_defaultNormalMap;
	//���̃x�^�h��e�N�X�`��
	static std::shared_ptr<TextureBuffer>s_defaultEmissiveMap;

public:
	static void SetEyePos(Vec3<float> EyePos);

private:
	//�萔�o�b�t�@���M�p�f�[�^
	struct ConstantData
	{
		Matrix m_mat;
		Color m_color;
		float z = 0.0f;
		float m_diffuse = 1.0f;	//�f�B�q���[�Y�̉e���x
		float m_specular = 1.0f;	//�X�y�L�����[�̉e���x
		float m_lim = 1.0f;	//�������C�g�̉e���x
	}m_constData;

	//�萔�o�b�t�@
	std::shared_ptr<ConstantBuffer>m_constBuff;

	//�e�N�X�`���o�b�t�@
	std::shared_ptr<TextureBuffer>m_texBuff;
	//�m�[�}���}�b�v
	std::shared_ptr<TextureBuffer>m_normalMap;
	//�G�~�b�V�u�}�b�v
	std::shared_ptr<TextureBuffer>m_emissiveMap;

public:
	//�g�����X�t�H�[��
	Transform2D m_transform;
	//���b�V���i���_���j
	SpriteMesh m_mesh;

	//�e�N�X�`���A�m�[�}���}�b�v�A�X�v���C�g��
	Sprite_Shadow(const std::shared_ptr<TextureBuffer>& Texture = nullptr, const std::shared_ptr<TextureBuffer>& NormalMap = nullptr,
		const std::shared_ptr<TextureBuffer>& EmissiveMap = nullptr, const char* Name = nullptr);

	//�e�N�X�`���ƃm�[�}���}�b�v�Z�b�g
	void SetTexture(const std::shared_ptr<TextureBuffer>& Texture, const std::shared_ptr<TextureBuffer>& NormalMap, const std::shared_ptr<TextureBuffer>& EmissiveMap);

	//�F�Z�b�g�i�`��̍ۂɂ��̒l����Z�����j
	void SetColor(const Color& Color);

	//�A�e������ۂɎg���[�x�l
	void SetDepth(const float& Depth);

	//���C�g�̉e���x�ݒ�
	void SetDiffuseAffect(const float& Diffuse);
	void SetSpecularAffect(const float& Specular);
	void SetLimAffect(const float& Lim);

	//�`��
	void Draw(LightManager& LigManager);
};

