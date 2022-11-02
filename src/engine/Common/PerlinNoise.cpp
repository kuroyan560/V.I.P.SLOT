#include "PerlinNoise.h"
#include<vector>
#include"KuroFunc.h"
#include"D3D12App.h"
#include<magic_enum.h>

int NoiseInitializer::ID = 0;

void PerlinNoise::DrawToTex(std::shared_ptr<TextureBuffer> DestTex, const NoiseInitializer& Config)
{
	//�ő啪����
	assert(0 < Config.m_split.x && Config.m_split.x <= SPLIT_MAX && 0 < Config.m_split.y && Config.m_split.y <= SPLIT_MAX);

	//��ԕ��@�̒l���K�؂�
	assert(0 <= Config.m_interpolation && Config.m_interpolation < NOISE_INTERPOLATION_NUM);

	//�R���s���[�g�p�C�v���C��
	static std::shared_ptr<ComputePipeline>PIPELINE;

	//�萔�o�b�t�@
	static std::shared_ptr<ConstantBuffer>CONST_BUFF;
	//�萔�o�b�t�@�p�f�[�^
	struct ConstData
	{
		Vec2<float>rectLength;
		Vec2<int> split;
		int interpolation;
		int contrast;
		int octaveNum;
		float frequency;
		float persistance;
		ConstData(const int& Interpolation, const Vec2<float>& RectLength, const Vec2<int>& Split, const int& Contrast, const int& Octaves, const float& Frequency, const float& Persistance)
			:interpolation(Interpolation), rectLength(RectLength), split(Split), contrast(Contrast), octaveNum(Octaves), frequency(Frequency), persistance(Persistance) {}
	};

	//�\���̃o�b�t�@
	static std::shared_ptr<StructuredBuffer>STRUCTURED_BUFF;

	if (!PIPELINE)
	{
		//�V�F�[�_
		auto cs = D3D12App::Instance()->CompileShader("resource/engine/PerlinNoise2D.hlsl", "CSmain", "cs_6_4");
		//���[�g�p�����[�^
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�t���N�^���p�[�����m�C�Y�������"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"���z�x�N�g���z��"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"�t���N�^���p�[�����m�C�Y�e�N�X�`��"),
		};
		//�p�C�v���C������
		PIPELINE = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams, { WrappedSampler(false, false) });
	}

	//�萔�o�b�t�@����
	if (!CONST_BUFF)
	{
		CONST_BUFF = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ConstData), 1, nullptr, "PerlinNoise - ConstantBuffer ");
	}

	//�\���̃o�b�t�@����
	if (!STRUCTURED_BUFF)
	{
		STRUCTURED_BUFF =
			D3D12App::Instance()->GenerateStructuredBuffer(
				sizeof(Vec2<float>),
				static_cast<int>(pow(SPLIT_MAX + 1, 2)),
				nullptr,
				"PerlinNoise2D - StructuredBuffer");
	}

	//�萔�o�b�t�@�Ƀf�[�^�]��
	ConstData constData(Config.m_interpolation, DestTex->GetGraphSize().Float() / Config.m_split.Float(), Config.m_split, Config.m_contrast, Config.m_octave, Config.m_frequency, Config.m_persistance);
	CONST_BUFF->Mapping(&constData);

	//������̊e���_�̌��z�x�N�g���i�[��
	Vec2<float>grad[(SPLIT_MAX + 1) * (SPLIT_MAX + 1)];
	for (int y = 0; y <= Config.m_split.y; ++y)
	{
		for (int x = 0; x <= Config.m_split.x; ++x)
		{
			int idx = y * (Config.m_split.x + 1) + x;
			//�����_���Ȍ��z�x�N�g��
			grad[idx].x = KuroFunc::GetRand(1.0f) * KuroFunc::GetRandPlusMinus();
			grad[idx].y = KuroFunc::GetRand(1.0f) * KuroFunc::GetRandPlusMinus();

			if (x == Config.m_split.x)grad[idx] = grad[y * (Config.m_split.x + 1)];
			if (y == Config.m_split.y)grad[idx] = grad[x];
		}
	}
	//�\�����o�b�t�@�ɓ]��
	STRUCTURED_BUFF->Mapping(grad);

	//�R�}���h���X�g�擾
	auto cmdList = D3D12App::Instance()->GetCmdList();

	//�f�B�X�N���v�^�q�[�v�Z�b�g
	D3D12App::Instance()->SetDescHeaps();

	//�p�C�v���C���Z�b�g
	PIPELINE->SetPipeline(cmdList);

	//�萔�o�b�t�@�Z�b�g
	CONST_BUFF->SetComputeDescriptorBuffer(cmdList, CBV, 0);

	//�\���̃o�b�t�@�Z�b�g
	STRUCTURED_BUFF->SetComputeDescriptorBuffer(cmdList, SRV, 1);

	//�`�����ݐ�e�N�X�`���o�b�t�@�Z�b�g
	DestTex->SetComputeDescriptorBuffer(cmdList, UAV, 2);


	//���s
	static const int THREAD_NUM = 16;
	const Vec2<UINT>thread =
	{
		static_cast<UINT>((DestTex->GetGraphSize().x / THREAD_NUM) + 1),
		static_cast<UINT>((DestTex->GetGraphSize().y / THREAD_NUM) + 1)
	};
	cmdList->Dispatch(thread.x, thread.y, 1);
}

std::shared_ptr<TextureBuffer> PerlinNoise::GenerateTex(const std::string& Name, const Vec2<int>& Size, const NoiseInitializer& Config, const DXGI_FORMAT& Format)
{
	//�`�����ݐ�p�e�N�X�`���o�b�t�@����
	auto result = D3D12App::Instance()->GenerateTextureBuffer(Size, Format, Name.c_str());
	DrawToTex(result, Config);
	return result;
}

#include"imguiApp.h"
bool PerlinNoise::ImguiDebug(NoiseInitializer& arg_initializer)
{
	bool changed = false;

	ImGui::Begin(("Noise setting - " + std::to_string(arg_initializer.m_id)).c_str());

	//��ԕ��@
	std::string preview = std::string(magic_enum::enum_name(arg_initializer.m_interpolation));
	if (ImGui::BeginCombo("NOISE_INTERPOLATION", preview.c_str()))
	{
		for (int i = 0; i < NOISE_INTERPOLATION_NUM; ++i)
		{
			bool isSelected = arg_initializer.m_interpolation == i;
			NOISE_INTERPOLATION nowInterpolation = (NOISE_INTERPOLATION)i;
			std::string current = std::string(magic_enum::enum_name(nowInterpolation));
			if (ImGui::Selectable(current.c_str(), isSelected))
			{
				arg_initializer.m_interpolation = nowInterpolation;
				changed = true;
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	//������
	if (ImGui::DragInt("SplitX", &arg_initializer.m_split.x, 1, 1,SPLIT_MAX))changed = true;
	if (ImGui::DragInt("SplitY", &arg_initializer.m_split.y, 1, 1,SPLIT_MAX))changed = true;

	if(ImGui::DragInt("Contrast", &arg_initializer.m_contrast))changed = true;
	if(ImGui::DragInt("Octave", &arg_initializer.m_octave))changed = true;
	if(ImGui::DragFloat("Frequency", &arg_initializer.m_frequency))changed = true;
	if(ImGui::DragFloat("Persistance", &arg_initializer.m_persistance))changed = true;

	ImGui::End();

	return changed;
}

void PerlinNoise::Initialize(const NoiseInitializer& arg_initializer)
{
	m_initializer = arg_initializer;
	m_invalid = false;
}

float PerlinNoise::Get(Vec2<float> arg_xy)
{
	if (m_invalid)
	{
		printf("[Caution] This PerlinNoise isn't initialized.\n");
		return 0.0f;
	}

}
