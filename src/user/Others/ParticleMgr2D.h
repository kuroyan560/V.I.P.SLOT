#pragma once
#include"Singleton.h"
#include<memory>
#include"Vec.h"
#include"Color.h"
#include<vector>
#include<string>
#include"KuroMath.h"
#include<map>
#include"D3D12Data.h"

class VertexBuffer;
class ComputePipeline;
class GraphicsPipeline;
class ConstantBuffer;
class TextureBuffer;

struct ParticleInitializer
{
	Color m_mulColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vec2<float>m_pos = { 0.0f,0.0f };
	Vec2<float>m_vec = { 0.0f,0.0f };
	float m_speed = 0.0f;
	float m_scale = 64.0f;
	float m_radian = 0.0f;
	float m_lifeSpan = 60.0f;
	unsigned int m_texIdx = 0;
};

class ParticleMgr2D : public Singleton<ParticleMgr2D>
{
	friend class Singleton<ParticleMgr2D>;
	ParticleMgr2D();

	//スレッド数
	static const int THREAD_PER_NUM = 10;
	//セットできるテクスチャの最大数
	static const int TEX_NUM_MAX = 10;

	struct Particle
	{
		ParticleInitializer m_initializer;	//初期化情報
		Color m_mulColor;	//乗算色
		Vec2<float>m_pos;	//座標
		float m_speed;	//スピード
		float m_scale;	//スケール
		float m_radian;	//回転角度
		float m_lifeTimer = 0;	//生成されてからのタイマー
		unsigned int m_isAlive = 0;		// 生存フラグ
		unsigned int m_texIdx;	//テクスチャインデックス

		// 生成処理
		void Generate(const ParticleInitializer& arg_initializer)
		{
			m_initializer = arg_initializer;
			m_mulColor = arg_initializer.m_mulColor;
			m_pos = arg_initializer.m_pos;
			m_speed = arg_initializer.m_speed;
			m_scale = arg_initializer.m_scale;
			m_radian = arg_initializer.m_radian;
			m_lifeTimer = 0.0f;
			m_isAlive = 1;
			m_texIdx = arg_initializer.m_texIdx;
		}
	};

	struct DefineParticleInfo
	{
		//パーティクルインスタンス
		std::vector<Particle>m_particles;
		//パーティクルの情報格納バッファ、描画に利用
		std::shared_ptr<VertexBuffer>m_vertBuff;
		//パーティクルの更新パイプライン
		std::shared_ptr<ComputePipeline>m_cPipeline;
		//テクスチャバッファ配列
		std::vector<std::shared_ptr<TextureBuffer>>m_textures;
		//デフォルトの初期化情報
		ParticleInitializer* m_defaultInitializer = nullptr;
		//次に放出するパーティクルのインデックス
		int m_nextEmitParticleIdx = 0;
		//描画時のブレンドモード
		AlphaBlendMode m_blendMode;

		~DefineParticleInfo()
		{
			if (m_defaultInitializer != nullptr)
			{
				delete m_defaultInitializer;
				m_defaultInitializer = nullptr;
			}
		}
	};
	std::vector<DefineParticleInfo>m_particleInfoArray;

	//全パーティクル共通の定数バッファ
	struct CommonConstData
	{
		float m_timeScale = 1.0f;
	}m_commonConstData;
	std::shared_ptr<ConstantBuffer>m_commonConstBuff;

	//全パーティクルの更新を行いコンピュートパイプライン
	std::shared_ptr<ComputePipeline>m_commonComputePipeline;

	//描画用グラフィックパイプライン
	std::map<AlphaBlendMode, std::shared_ptr<GraphicsPipeline>>m_graphicsPipeline;

public:
	void Init();
	void Update(float arg_timeScale);
	void Draw();

	/// <summary>
	/// パーティクル準備
	/// </summary>
	/// <param name="arg_maxInstanceNum">最大数</param>
	/// <param name="arg_computeShaderPath">パーティクルの更新を行うコンピュートシェーダーのファイルパス</param>
	/// <param name="arg_texArray">テクスチャ配列先頭ポインタ</param>
	/// <param name="arg_texArraySize">テクスチャ配列サイズ</param>
	/// <param name="arg_defaultInitializer">デフォルトの初期化情報</param>
	/// <param name="arg_blendMode">描画時のブレンドモード</param>
	/// <returns>パーティクルID：パーティクル情報のインデックス(Emit時に必要)</returns>
	int Prepare(int arg_maxInstanceNum,
		std::string arg_computeShaderPath,
		std::shared_ptr<TextureBuffer>* arg_texArray,
		size_t arg_texArraySize,
		ParticleInitializer* arg_defaultInitializer = nullptr,
		AlphaBlendMode arg_blendMode = AlphaBlendMode_Trans);

	/// <summary>
	/// パーティクル生成
	/// </summary>
	/// <param name="arg_particleID">パーティクルID</param>
	/// <param name="arg_initializer">初期化情報</param>
	/// <param name="arg_texIdx">参照するテクスチャのインデックス</param>
	void Emit(int arg_particleID, ParticleInitializer arg_initializer);

	/// <summary>
	/// 設定されているデフォルトの初期化情報のゲッタ
	/// </summary>
	/// <param name="arg_particleIdx">パーティクルID</param>
	/// <returns>設定されているデフォルトの初期化情報</returns>
	const ParticleInitializer& GetDefaultInitializer(int arg_particleIdx)const
	{
		assert(m_particleInfoArray[arg_particleIdx].m_defaultInitializer != nullptr);
		return *m_particleInfoArray[arg_particleIdx].m_defaultInitializer;
	}

	/// <summary>
	/// 指定した種別のパーティクルをすべて消す
	/// </summary>
	/// <param name="arg_particleID">パーティクルID</param>
	void EraseParticles(int arg_particleID);

	/// <summary>
	/// 全てのパーティクルを消す
	/// </summary>
	void EraseAllParticles()
	{
		for (int i = 0; i < static_cast<int>(m_particleInfoArray.size()); ++i)EraseParticles(i);
	}
};