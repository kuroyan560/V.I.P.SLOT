#pragma once
#include<memory>
#include"Vec.h"
#include<array>
#include<string>
#include"D3D12Data.h"

enum NOISE_INTERPOLATION
{
	WAVELET, BLOCK, NOISE_INTERPOLATION_NUM
};

struct NoiseInitializer
{
private:
	static int ID;
public:
	int m_id = ID++;
	NOISE_INTERPOLATION m_interpolation = WAVELET;
	Vec2<int>m_split = { 16,16 };
	int m_contrast = 1;
	int m_octave = 1;
	float m_frequency = 1.0f;
	float m_persistance = 0.5f;
};

class PerlinNoise
{
private:
	static const int SPLIT_MAX = 256;

	//�m�C�Y�̕�ԕ��@���Q�b�^
	static const std::string& GetInterpolationName(const NOISE_INTERPOLATION& Interpolation)
	{
		static std::array<std::string, NOISE_INTERPOLATION_NUM>NAME =
		{
			"Wavelet","Block"
		};
		return NAME[Interpolation];
	}
public:
	//�e�N�X�`���Ƀp�[�����m�C�Y�`��
	static void DrawToTex(std::shared_ptr<TextureBuffer>DestTex, const NoiseInitializer& Config);
	//�p�[�����m�C�Y�e�N�X�`������
	static std::shared_ptr<TextureBuffer>GenerateTex(const std::string& Name, const Vec2<int>& Size, const NoiseInitializer& Config, const DXGI_FORMAT& Format = DXGI_FORMAT_R32_FLOAT);
	//�m�C�Y�C�j�V�����C�U�[��imgui�f�o�b�O
	static bool ImguiDebug(NoiseInitializer& arg_initializer);


private:
	bool m_invalid = true;
	NoiseInitializer m_initializer;

public:
	void Initialize(const NoiseInitializer& arg_initializer);
	float Get(Vec2<float>arg_xy);
};