#pragma once
#include<memory>
#include"D3D12Data.h"
#include<array>
#include"SpriteMesh.h"

class GaussianBlur
{
private:
	enum PROCESS { X_BLUR, Y_BLUR, FINAL, PROCESS_NUM };
	static const int s_weightNum = 8;
	static void GeneratePipeline(std::array<std::shared_ptr<GraphicsPipeline>, PROCESS_NUM - 1>& arg_destPipeline, const DXGI_FORMAT& arg_format);

private:
	//�d�݃e�[�u��
	float m_weights[s_weightNum];

	//�X�v���C�g���b�V��
	std::array<std::unique_ptr<SpriteMesh>, PROCESS_NUM - 1>m_spriteMeshes;

	//�d�݃e�[�u���̒萔�o�b�t�@
	std::shared_ptr<ConstantBuffer>m_weightConstBuff;

	//�u���[�̌���
	std::array<std::shared_ptr<RenderTarget>, PROCESS_NUM>m_blurResult;

	//�p�C�v���C��
	std::array<std::shared_ptr<GraphicsPipeline>, PROCESS_NUM - 1>m_gPipeline;

public:
	//�\�[�X�摜 & ���ʕ`���ݒ�A�ڂ�������
	GaussianBlur(const Vec2<int>& arg_size, const DXGI_FORMAT& arg_format, const float& arg_blurPower = 8.0f);
	//�{�P�
	void SetBlurPower(const float& arg_blurPower);
	//�O���t�B�b�N�X�}�l�[�W���ɓo�^
	void Register(const std::shared_ptr<TextureBuffer>& arg_srcTex);

	//���ʂ̃e�N�X�`���擾
	std::shared_ptr<RenderTarget>& GetResultTex() { return m_blurResult[FINAL]; }

	//�E�B���h�E�T�C�Y�Ō��ʂ�`��
	void DrawResult(const AlphaBlendMode& arg_alphaBlend);
};