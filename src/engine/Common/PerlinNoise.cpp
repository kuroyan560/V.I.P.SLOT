#include "PerlinNoise.h"
#include<vector>
#include"KuroFunc.h"
#include"D3D12App.h"
#include<magic_enum.h>

int NoiseInitializer::ID = 0;

void PerlinNoise::DrawToTex(std::shared_ptr<TextureBuffer> DestTex, const NoiseInitializer& Config)
{
	//最大分割数
	assert(0 < Config.m_split.x && Config.m_split.x <= SPLIT_MAX && 0 < Config.m_split.y && Config.m_split.y <= SPLIT_MAX);

	//補間方法の値が適切か
	assert(0 <= Config.m_interpolation && Config.m_interpolation < NOISE_INTERPOLATION_NUM);

	//コンピュートパイプライン
	static std::shared_ptr<ComputePipeline>PIPELINE;

	//定数バッファ
	static std::shared_ptr<ConstantBuffer>CONST_BUFF;
	//定数バッファ用データ
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

	//構造体バッファ
	static std::shared_ptr<StructuredBuffer>STRUCTURED_BUFF;

	if (!PIPELINE)
	{
		//シェーダ
		auto cs = D3D12App::Instance()->CompileShader("resource/engine/PerlinNoise2D.hlsl", "CSmain", "cs_6_4");
		//ルートパラメータ
		std::vector<RootParam>rootParams =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"フラクタルパーリンノイズ生成情報"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"勾配ベクトル配列"),
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"フラクタルパーリンノイズテクスチャ"),
		};
		//パイプライン生成
		PIPELINE = D3D12App::Instance()->GenerateComputePipeline(cs, rootParams, { WrappedSampler(false, false) });
	}

	//定数バッファ生成
	if (!CONST_BUFF)
	{
		CONST_BUFF = D3D12App::Instance()->GenerateConstantBuffer(sizeof(ConstData), 1, nullptr, "PerlinNoise - ConstantBuffer ");
	}

	//構造体バッファ生成
	if (!STRUCTURED_BUFF)
	{
		STRUCTURED_BUFF =
			D3D12App::Instance()->GenerateStructuredBuffer(
				sizeof(Vec2<float>),
				static_cast<int>(pow(SPLIT_MAX + 1, 2)),
				nullptr,
				"PerlinNoise2D - StructuredBuffer");
	}

	//定数バッファにデータ転送
	ConstData constData(Config.m_interpolation, DestTex->GetGraphSize().Float() / Config.m_split.Float(), Config.m_split, Config.m_contrast, Config.m_octave, Config.m_frequency, Config.m_persistance);
	CONST_BUFF->Mapping(&constData);

	//分割後の各頂点の勾配ベクトル格納先
	Vec2<float>grad[(SPLIT_MAX + 1) * (SPLIT_MAX + 1)];
	for (int y = 0; y <= Config.m_split.y; ++y)
	{
		for (int x = 0; x <= Config.m_split.x; ++x)
		{
			int idx = y * (Config.m_split.x + 1) + x;
			//ランダムな勾配ベクトル
			grad[idx].x = KuroFunc::GetRand(1.0f) * KuroFunc::GetRandPlusMinus();
			grad[idx].y = KuroFunc::GetRand(1.0f) * KuroFunc::GetRandPlusMinus();

			if (x == Config.m_split.x)grad[idx] = grad[y * (Config.m_split.x + 1)];
			if (y == Config.m_split.y)grad[idx] = grad[x];
		}
	}
	//構造化バッファに転送
	STRUCTURED_BUFF->Mapping(grad);

	//コマンドリスト取得
	auto cmdList = D3D12App::Instance()->GetCmdList();

	//ディスクリプタヒープセット
	D3D12App::Instance()->SetDescHeaps();

	//パイプラインセット
	PIPELINE->SetPipeline(cmdList);

	//定数バッファセット
	CONST_BUFF->SetComputeDescriptorBuffer(cmdList, CBV, 0);

	//構造体バッファセット
	STRUCTURED_BUFF->SetComputeDescriptorBuffer(cmdList, SRV, 1);

	//描き込み先テクスチャバッファセット
	DestTex->SetComputeDescriptorBuffer(cmdList, UAV, 2);


	//実行
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
	//描き込み先用テクスチャバッファ生成
	auto result = D3D12App::Instance()->GenerateTextureBuffer(Size, Format, Name.c_str());
	DrawToTex(result, Config);
	return result;
}

#include"imguiApp.h"
bool PerlinNoise::ImguiDebug(NoiseInitializer& arg_initializer)
{
	bool changed = false;

	ImGui::Begin(("Noise setting - " + std::to_string(arg_initializer.m_id)).c_str());

	//補間方法
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

	//分割数
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
