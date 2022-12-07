#include "ParticleMgr2D.h"
#include"KuroEngine.h"

ParticleMgr2D::ParticleMgr2D()
{
	//DirectX機構取得
	auto d3d12app = D3D12App::Instance();

	//全パーティクル共通の定数バッファ用意
	m_commonConstBuff = d3d12app->GenerateConstantBuffer(
		sizeof(CommonConstData),
		1,
		&m_commonConstData,
		"ParticleMgr2D - CommonConstantBuffer");

	//コンピュートシェーダーのルートパラメータ（共通）
	static const std::vector<RootParam>ROOT_PARAMS =
	{
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"タイムスケール"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"パーティクル配列"),
	};

	//全パーティクルの更新を行いコンピュートパイプライン
	m_commonComputePipeline = d3d12app->GenerateComputePipeline(
		d3d12app->CompileShader("resource/user/shaders/Particle/ParticleMgr_Compute.hlsl", "CSmain", "cs_6_4"),
		ROOT_PARAMS,
		{ WrappedSampler(false, false) });


	//描画用グラフィックパイプライン
	{
		//パイプライン設定
		static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		PIPELINE_OPTION.m_depthTest = false;

		//シェーダー情報
		static Shaders SHADERS;
		SHADERS.m_vs = d3d12app->CompileShader("resource/user/shaders/Particle/ParticleMgr_Graphics.hlsl", "VSmain", "vs_6_4");
		SHADERS.m_gs = d3d12app->CompileShader("resource/user/shaders/Particle/ParticleMgr_Graphics.hlsl", "GSmain", "gs_6_4");
		SHADERS.m_ps = d3d12app->CompileShader("resource/user/shaders/Particle/ParticleMgr_Graphics.hlsl", "PSmain", "ps_6_4");

		//インプットレイアウト
		static std::vector<InputLayoutParam>INPUT_LAYOUT =
		{
			//初期化情報
			InputLayoutParam("EMIT_MUL_COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
			InputLayoutParam("EMIT_POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EMIT_VEC",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("EMIT_SPEED",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIFE_SPAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("EMIT_TEX_IDX",DXGI_FORMAT_R32_UINT),

			//パラメータ
			InputLayoutParam("MUL_COLOR",DXGI_FORMAT_R32G32B32A32_FLOAT),
			InputLayoutParam("POSITION",DXGI_FORMAT_R32G32_FLOAT),
			InputLayoutParam("SPEED",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("SCALE",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("RADIAN",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("LIFE_TIMER",DXGI_FORMAT_R32_FLOAT),
			InputLayoutParam("ALIVE",DXGI_FORMAT_R16_UINT),
			InputLayoutParam("TEX_IDX",DXGI_FORMAT_R32_UINT),
		};

		//ルートパラメータ
		//平行投影行列定数バッファ
		static std::vector<RootParam>ROOT_PARAMETER =
		{
			RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"平行投影行列定数バッファ"),
		};
		//テクスチャ配列
		for (int i = 0; i < TEX_NUM_MAX; ++i)
		{
			ROOT_PARAMETER.emplace_back(RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, ("テクスチャ情報 - " + std::to_string(i)).c_str()));
		}

		//レンダーターゲット描画先情報
		static std::vector<RenderTargetInfo>RENDER_TARGET_INFO =
		{
			//バックバッファのフォーマット、アルファブレンド
			RenderTargetInfo(d3d12app->GetBackBuffFormat(),AlphaBlendMode_Trans),
		};

		//パイプライン生成
		m_graphicsPipeline = d3d12app->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, INPUT_LAYOUT, ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });
	}
}

void ParticleMgr2D::Init()
{
	EraseAllParticles();
}

void ParticleMgr2D::Update(float arg_timeScale)
{
	//全パーティクル共通の定数バッファ更新
	if (m_commonConstData.m_timeScale != arg_timeScale)
	{
		m_commonConstData.m_timeScale = arg_timeScale;
		m_commonConstBuff->Mapping(&m_commonConstData);
	}

	

	//パーティクル種別ごとにコンピュートシェーダーで更新
	for (auto& info : m_particleInfoArray)
	{
		//コンピュートパイプラインにセットするディスクリプタ情報
		std::vector<RegisterDescriptorData>descDatas =
		{
			{m_commonConstBuff,CBV},
			{info.m_vertBuff->GetRWStructuredBuff().lock(),UAV}
		};

		D3D12App::Instance()->DispathOneShot(
			info.m_cPipeline,
			Vec3<int>(static_cast<int>(info.m_particles.size()) / THREAD_PER_NUM + 1, 1, 1),
			descDatas);
	}

	//全パーティクル共通の処理
	for (auto& info : m_particleInfoArray)
	{
		//コンピュートパイプラインにセットするディスクリプタ情報
		std::vector<RegisterDescriptorData>descDatas =
		{
			{m_commonConstBuff,CBV},
			{info.m_vertBuff->GetRWStructuredBuff().lock(),UAV}
		};
		D3D12App::Instance()->DispathOneShot(
			m_commonComputePipeline,
			Vec3<int>(static_cast<int>(info.m_particles.size()) / THREAD_PER_NUM + 1, 1, 1),
			descDatas);
	}


}

#include"LightManager.h"
void ParticleMgr2D::Draw()
{
	//グラフィックス統括取得
	auto& graphics = KuroEngine::Instance()->Graphics();

	//パーティクル種別ごとに描画
	for (auto& info : m_particleInfoArray)
	{
		//平行投影行列バッファ
		std::vector<RegisterDescriptorData>descDatas =
		{
			{KuroEngine::Instance()->GetParallelMatProjBuff(),CBV},
		};

		//テクスチャ配列
		for (auto& tex : info.m_textures)
		{
			descDatas.push_back({ tex ,SRV });
		}

		graphics.SetGraphicsPipeline(m_graphicsPipeline);
		graphics.ObjectRender(
			info.m_vertBuff,
			descDatas,
			1.0f,
			true,
			1);
	}
}

int ParticleMgr2D::Prepare(int arg_maxInstanceNum, std::string arg_computeShaderPath, std::shared_ptr<TextureBuffer>* arg_texArray, size_t arg_texArraySize, ParticleInitializer* arg_defaultInitializer)
{
	//セットできるテクスチャの最大数を超えている
	assert(static_cast<int>(arg_texArraySize) < TEX_NUM_MAX);
	
	//配列のインデックスをパーティクルIDとする
	int particleID = static_cast<int>(m_particleInfoArray.size());

	//パーティクル情報追加
	m_particleInfoArray.emplace_back();

	//参照取得
	auto& info = m_particleInfoArray.back();
	//DirectX機構取得
	auto d3d12App = D3D12App::Instance();

	//最大インスタンス数分パーティクル用意
	info.m_particles.resize(arg_maxInstanceNum);

	//パーティクル情報を頂点バッファとして生成、送信
	info.m_vertBuff = d3d12App->GenerateVertexBuffer(
		sizeof(Particle),
		arg_maxInstanceNum,
		info.m_particles.data(),
		("ParticleMgr - VertexBuffer - " + std::to_string(particleID)).c_str(),
		true);

	//コンピュートシェーダーのルートパラメータ（共通）
	static const std::vector<RootParam>ROOT_PARAMS =
	{
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"タイムスケール"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_UAV,"パーティクル配列"),
	};
	//コンピュートシェーダー生成
	info.m_cPipeline = d3d12App->GenerateComputePipeline(
		d3d12App->CompileShader(arg_computeShaderPath, "CSmain", "cs_6_4"),
		ROOT_PARAMS,
		{ WrappedSampler(false, false) });

	//テクスチャ配列を記録
	for (int i = 0; i < static_cast<int>(arg_texArraySize); ++i)
	{
		info.m_textures.emplace_back(arg_texArray[i]);
	}

	//デフォルトの初期化情報があれば記録
	if (arg_defaultInitializer != nullptr)
	{
		info.m_defaultInitializer = new ParticleInitializer(*arg_defaultInitializer);
	}

	return particleID;
}

void ParticleMgr2D::Emit(int arg_particleID, ParticleInitializer arg_initializer)
{
	auto& info = m_particleInfoArray[arg_particleID];

	//パーティクル放出
	info.m_particles[info.m_nextEmitParticleIdx].Generate(arg_initializer);
	//GPUに送信
	info.m_vertBuff->Mapping(&info.m_particles[info.m_nextEmitParticleIdx], 1, info.m_nextEmitParticleIdx);

	//次に放出するパーティクルのインデックスをインクリメント、最後のパーティクルまできたら0番目に戻る
	if (static_cast<int>(info.m_particles.size()) <= ++info.m_nextEmitParticleIdx)info.m_nextEmitParticleIdx = 0;
}

void ParticleMgr2D::EraseParticles(int arg_particleID)
{
	auto& info = m_particleInfoArray[arg_particleID];

	//次に放出するパーティクルのインデックスを初期化
	info.m_nextEmitParticleIdx = 0;

	//全てのパーティクルの生存フラグをOFFに
	for (auto& particle : info.m_particles)
	{
		particle.m_isAlive = 0;
	}

	//バッファ送信
	info.m_vertBuff->Mapping(info.m_particles.data());
}
