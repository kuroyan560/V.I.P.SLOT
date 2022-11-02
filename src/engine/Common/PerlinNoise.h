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

	//�����̐����������_���ȃn�b�V���l�ɕԊ�
	static unsigned xorshift32(unsigned v)
	{
		v = v ^ (v << 13);
		v = v ^ (v << 17);
		v = v ^ (v << 15);
		return v;
	}

	static float Wavelet(float t)
	{
		return static_cast<float>(1 - (6 * abs(pow(t, 5)) - 15 * pow(t, 4) + 10 * abs(pow(t, 3))));
	}

	//���z��ݒ肵���E�F�[�u���b�g�֐�
	static float GradWaveLet(Vec2<float>arg_uv, Vec2<float> arg_grad)
	{
		float l = arg_uv.x * arg_grad.x + arg_uv.y * arg_grad.y;
		float c = Wavelet(arg_uv.x) * Wavelet(arg_uv.y);
		return c * l;
	}

	/// <summary>
	/// ���z�������擾
	/// </summary>
	/// <param name="x">���E�������WX</param>
	/// <param name="y">���E�������WY</param>
	/// <param name="elem">���z�̎��ԍ��ix��->0,y��->1�Ȃǁj</param>
	/// <returns></returns>
	static float GetGrad(int arg_x, int arg_y, int arg_elem, int arg_seed = 0);

public:
	//�e�N�X�`���Ƀp�[�����m�C�Y�`��
	static void DrawToTex(std::shared_ptr<TextureBuffer>DestTex, const NoiseInitializer& Config);
	//�p�[�����m�C�Y�e�N�X�`������
	static std::shared_ptr<TextureBuffer>GenerateTex(const std::string& Name, const Vec2<int>& Size, const NoiseInitializer& Config, const DXGI_FORMAT& Format = DXGI_FORMAT_R32_FLOAT);
	//�m�C�Y�C�j�V�����C�U�[��imgui�f�o�b�O
	static bool ImguiDebug(NoiseInitializer& arg_initializer);

	//�p�[�����m�C�Y���痐���擾
	static float GetRand(float arg_x, float arg_y, int arg_seed = 0);
};