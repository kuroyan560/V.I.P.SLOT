#include "PerlinNoise.h"
#include<vector>
#include"KuroFunc.h"
#include"D3D12App.h"
#include<magic_enum.h>

int NoiseInitializer::ID = 0;

float PerlinNoise::GetGrad(int arg_x, int arg_y, int arg_elem, int arg_seed)
{
	static std::array<int, 256> HASH;
	static bool HASH_INIT = false;
	if (!HASH_INIT)
	{
		int idx = 0;
		for (auto& key : HASH)key = idx++;

		for (int i = static_cast<int>(HASH.size()) - 1; 0 < i; --i)
		{
			int r = static_cast<int>(floor(KuroFunc::GetRand(1.0f) * (i + 1)));
			auto tmp = HASH[i];
			HASH[i] = HASH[r];
			HASH[r] = tmp;
		}
		HASH_INIT = true;
	}

	int hashX = HASH[arg_x % 256];
	int hashXY = HASH[(hashX + arg_y) % 256];
	int hashGrad = HASH[(hashXY + arg_elem) % 256];

	unsigned r = xorshift32(arg_x + arg_seed);
	r = xorshift32(r + arg_y);
	r = xorshift32(r + arg_elem) % 10000;
	return (float)r / 5000 - 1.0f;

	//-1~1�͈̔͂�Ԃ�
	return (hashGrad / 255.0f) * 2.0f - 1.0f;
}

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

float PerlinNoise::GetRand(float arg_x, float arg_y, int arg_seed)
{
	int floorX = static_cast<int>(floor(arg_x));
	int ceilX = static_cast<int>(ceil(arg_x));
	if (floorX == ceilX)ceilX++;
	int floorY = static_cast<int>(floor(arg_y));
	int ceilY = static_cast<int>(ceil(arg_y));
	if (floorY == ceilY)ceilY++;

	//���E�������W
	Vec2<int>upL = { floorX,floorY };
	Vec2<int>upR = { ceilX,floorY };
	Vec2<int>bottomL = { floorX,ceilY };
	Vec2<int>bottomR = { ceilX,ceilY };

	//���z�x�N�g���擾
	Vec2<float>gradUpL = { GetGrad(upL.x,upL.y,0,arg_seed),GetGrad(upL.x,upL.y,1,arg_seed) };
	Vec2<float>gradUpR = { GetGrad(upR.x,upR.y,0,arg_seed),GetGrad(upR.x,upR.y,1,arg_seed) };
	Vec2<float>gradBottomL = { GetGrad(bottomL.x,bottomL.y,0,arg_seed),GetGrad(bottomL.x,bottomL.y,1,arg_seed) };
	Vec2<float>gradBottomR = { GetGrad(bottomR.x,bottomR.y,0,arg_seed),GetGrad(bottomR.x,bottomR.y,1,arg_seed) };

	Vec2<float>uv = { arg_x - static_cast<int>(arg_x) ,arg_y - static_cast<int>(arg_y) };

	//����ƉE��̑΂ŕ��
	float wUpL = GradWaveLet(uv, gradUpL);
	float wUpR = GradWaveLet(Vec2<float>(-1.0f + uv.x, uv.y), gradUpR);
	float wUp = KuroMath::Lerp(wUpL, wUpR, uv.x);

	//�����ƉE���̑΂ŕ��
	float wBottomL = GradWaveLet(Vec2<float>(uv.x, -1.0f + uv.y), gradBottomL);
	float wBottomR = GradWaveLet(Vec2<float>(-1.0f + uv.x, -1.0f + uv.y), gradBottomR);
	float wBottom = KuroMath::Lerp(wBottomL, wBottomR, uv.x);

	//Y�������ɕ��
	return KuroMath::Lerp(wUp, wBottom, uv.y);
}