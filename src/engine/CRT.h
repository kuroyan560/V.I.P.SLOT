#pragma once
#include"Vec.h"
#include<memory>
#include"D3D12Data.h"
#include<map>
class SpriteMesh;

class CRT
{
	struct Info
	{
		//��ʃT�C�Y
		float m_screenSizeX = 1280.0f;
		float m_screenSizeY = 720.0f;
		//��ʘc�݋���
		float m_screenDistort = 0.1f;
		float pad;
	}m_crtInfo;

	std::shared_ptr<SpriteMesh>m_spriteMesh;
	std::shared_ptr<ConstantBuffer>m_crtInfoBuff;
	std::shared_ptr<RenderTarget>m_resultTex;

public:
	CRT();

	void Update();

	//�O���t�B�b�N�X�}�l�[�W���ɓo�^
	void Register(const std::shared_ptr<TextureBuffer>& arg_srcTex);
	//���ʂ̃e�N�X�`���擾
	std::shared_ptr<RenderTarget>& GetResultTex() { return m_resultTex; }
	//�E�B���h�E�T�C�Y�Ō��ʂ�`��
	void DrawResult(const AlphaBlendMode& AlphaBlend);
};