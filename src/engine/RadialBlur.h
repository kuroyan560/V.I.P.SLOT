#pragma once
#pragma once
#include<wrl.h>
#include<memory>

class ComputePipeline;
class ConstantBuffer;
class TextureBuffer;
class RenderTarget;

class RadialBlur
{
private:
	static std::shared_ptr<ComputePipeline>RADIAL_PIPELINE;
	static void GeneratePipeline();

private:
	//�萔�o�b�t�@�p�f�[�^
	struct ConstData
	{
		Vec2<int>texSize;
		float blurPower = 20.0f;
		Vec2<float>centerUV = { 0.5f,0.5f };
		float pad[3];
	}constData;

	//�萔�o�b�t�@
	std::shared_ptr<ConstantBuffer>constBuff;

	//���ʂ̃e�N�X�`��
	std::shared_ptr<TextureBuffer>finalResult;

public:
	//�\�[�X�摜 & ���ʕ`���ݒ�A�ڂ�������
	RadialBlur(const Vec2<int>& Size, const DXGI_FORMAT& Format, const float& BlurPower = 8.0f, const Vec2<float>& CenterUV = { 0.5f,0.5f });
	//�{�P�
	void SetBlurPower(const float& BlurPower);
	//�u���[�̒��S�_UV
	void SetCenterUV(const Vec2<float>& CenterUV = { 0.5f,0.5f });
	
	//���s
	void Excute(const Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& CmdList, const std::shared_ptr<TextureBuffer>& SourceTex);

	//���ʂ̃e�N�X�`���擾
	std::shared_ptr<TextureBuffer>& GetResultTex() { return finalResult; }
};