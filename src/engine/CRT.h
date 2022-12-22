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
		float m_noiseX = 0.0f;
		float m_rgbNoise = 0.0f;
		//float sinNoiseScale = 1.0f;
		float m_sinNoiseScale = 0.0f;
		//float sinNoiseWidth = 1.0f;
		float m_sinNoiseWidth = 0.0f;

		//float sinNoiseOffset = 1.0f;
		float m_sinNoiseOffset = 0.0f;
		float m_scanLineSpeed = 100.0f;
		float m_scanLineTail = 0.5f;
		float m_time = 0;

		Vec2<float>m_offset = { 0,0 };
		float m_screenParamX;
		float m_pad;
		//float barrelPower = 1;
	}m_crtInfo;

	std::shared_ptr<SpriteMesh>m_spriteMesh;
	std::shared_ptr<ConstantBuffer>m_crtInfoBuff;
	std::shared_ptr<RenderTarget>m_resultTex;

public:
	CRT();

	//�O���t�B�b�N�X�}�l�[�W���ɓo�^
	void Register(const std::shared_ptr<TextureBuffer>& arg_srcTex);
	//���ʂ̃e�N�X�`���擾
	std::shared_ptr<RenderTarget>& GetResultTex() { return m_resultTex; }
	//�E�B���h�E�T�C�Y�Ō��ʂ�`��
	void DrawResult(const AlphaBlendMode& AlphaBlend);
};