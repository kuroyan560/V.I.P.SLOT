#pragma once
#include<memory>
class RenderTarget;
class ComputePipeline;
class ConstantBuffer;
class GaussianBlur;
class TextureBuffer;

//DOF�ݒ���
struct DOFConfig
{
	//��O
	float m_nearPint = 0.0f;
	//���i��O�`���܂Ń{�P�Ȃ��j
	float m_farPint  = 10.0f;
	//��O�ȍ~�A���ȍ~����{�P�ő�ɂȂ�܂ł̋���
	float m_pintLength = 20.0f;
};

class DOF
{
private:
	static std::shared_ptr<ComputePipeline>s_pipeline;
	void GeneratePipeline();

private:
	//DOF�ݒ���
	DOFConfig m_config;
	std::shared_ptr<ConstantBuffer>m_configBuff;

	//�K�E�V�A���u���[
	std::shared_ptr<GaussianBlur>m_gaussianBlur;

	//�[�x�}�b�v�����Ƃɐ����������߃{�P�摜�̊i�[��
	std::shared_ptr<TextureBuffer>m_processedTex;

public:
	DOF();
	void SetPintConfig(float FrontPint = 0.0f, float BackPint = 10.0f, float PintLength = 20.0f);
	void Draw(std::weak_ptr<RenderTarget>Src, std::weak_ptr<RenderTarget>DepthMap);
};

