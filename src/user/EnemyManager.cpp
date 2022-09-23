#include "EnemyManager.h"
#include"Importer.h"
#include"Enemy.h"
#include"KuroEngine.h"
#include"Model.h"
#include"ModelAnimator.h"
#include"Camera.h"
#include"CubeMap.h"
#include"Collision.h"
#include"Collider.h"

EnemyManager::EnemyManager()
{
/*--- パイプライン生成 ---*/
	//パイプライン設定
	static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//シェーダー情報
	static Shaders SHADERS;
	SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/user/shaders/Enemy.hlsl", "VSmain", "vs_6_4");
	SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/user/shaders/Enemy.hlsl", "PSmain", "ps_6_4");

	//ルートパラメータ
	static std::vector<RootParam>ROOT_PARAMETER =
	{
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"カメラ情報バッファ"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"マテリアル基本情報バッファ"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"トランスフォームバッファ配列（構造化バッファ）"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ボーン行列バッファ（構造化バッファ）"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "キューブマップ"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"ベースカラーテクスチャ"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"メタルネステクスチャ"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"粗さ"),
	};

	//レンダーターゲット描画先情報
	std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None) };

	//パイプライン生成
	m_pipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });

/*--- エネミー型オブジェクト定義 ---*/
	{
		//サンドバッグ（何もしてこない）
		std::string name = "SandBag";

		auto model = Importer::Instance()->LoadModel("resource/user/", "sandbag.glb");
		std::vector<EnemyActPointInfo> actPoints =
		{
			EnemyActPointInfo(10),
		};

		//コライダー生成
		const float COLLIDER_RADIUS = 7.0f;
		auto colSphere = std::make_shared<CollisionSphere>(COLLIDER_RADIUS, Vec3<float>(0, 0, 0));
		auto col = Collider::Generate(
			name + "'s Colldier",
			colSphere,
			EnemyBreed::GetDamageCallBack(),
			COLLIDER_ATTRIBUTE::ENEMY,
			COLLIDER_ATTRIBUTE::PLAYER | COLLIDER_ATTRIBUTE::PLAYER_ATTACK);

		//あくまでクローン元なので非アクティブにする
		std::vector<std::shared_ptr<Collider>>colliders = { col };
		for (auto& col : colliders)col->SetActive(false);

		m_breeds[SANDBAG] = std::make_unique<EnemyBreed>(
			name,
			100,
			model,
			actPoints,
			colliders,
			Vec3<float>(0.0f, -3.5f, 0.0f));
	}
}

void EnemyManager::Clear()
{
	//生成済エネミー削除
	for (int enemyType = 0; enemyType < ENEMY_TYPE_NUM; ++enemyType)
	{
		EnemyArray& enemyArray = m_enemys[enemyType];
		enemyArray.clear();
	}
}

void EnemyManager::Init()
{
	//生成済エネミー削除
	for (int enemyType = 0; enemyType < ENEMY_TYPE_NUM; ++enemyType)
	{
		EnemyArray& enemyArray = m_enemys[enemyType];
		for (auto& enemy : enemyArray)
		{
			enemy->Init();
		}
	}
}

void EnemyManager::Update(const Player& Player, const float& Gravity)
{
	for (int enemyType = 0; enemyType < ENEMY_TYPE_NUM; ++enemyType)
	{
		EnemyArray& enemyArray = m_enemys[enemyType];

		//存在しないならスルー
		if (enemyArray.empty())continue;

		//更新
		for (auto& enemy : enemyArray)
		{
			enemy->Update(Player, Gravity);
		}

		//死んでたら削除
		auto result = std::remove_if(enemyArray.begin(), enemyArray.end(), [](const std::shared_ptr<Enemy>& enemy)
			{
				return !enemy->IsAlive();
			});
		enemyArray.erase(result, enemyArray.end());
	}
}

void EnemyManager::Draw(Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap)
{
	KuroEngine::Instance()->Graphics().SetGraphicsPipeline(m_pipeline);

	for (int enemyType = 0; enemyType < ENEMY_TYPE_NUM; ++enemyType)
	{
		//敵配列取得
		EnemyArray& enemyArray = m_enemys[enemyType];

		//存在しないならスルー
		if (enemyArray.empty())continue;

		//モデル取得
		auto& model = m_breeds[enemyType]->GetModel();

		//ワールド行列類更新
		static std::vector<Matrix>WORLD_MATRICIES(s_maxNum);	//行列配列は使いまわし
		std::fill(WORLD_MATRICIES.begin(), WORLD_MATRICIES.end(), XMMatrixIdentity());
		for (int enemyIdx = 0; enemyIdx < enemyArray.size(); ++enemyIdx)
		{
			WORLD_MATRICIES[enemyIdx] = enemyArray[enemyIdx]->GetParentMat();
		}
		m_worldMatriciesBuff[enemyType]->Mapping(WORLD_MATRICIES.data());

		//ボーン行列更新
		/*
		if (boneMatriciesBuff[enemyType])	//ボーン関連情報を持っているか
		{
			const int boneNum = model->skelton->bones.size() - 1;
			std::vector<Matrix>boneMatricies;
			for (auto& enemy : enemyArray)
			{
				const auto& boneAnimMat = enemy->GetAnimator()->GetBoneMatricies();
				for (auto& mat : boneAnimMat)boneMatricies.emplace_back(mat);
			}
			boneMatriciesBuff[enemyType]->Mapping(boneMatricies.data());
		}
		*/

		const int enemyNum = static_cast<int>(enemyArray.size());	//インスタンス数取得

		for (auto& mesh : model->m_meshes)
		{
			KuroEngine::Instance()->Graphics().ObjectRender(
				mesh.mesh->vertBuff,
				mesh.mesh->idxBuff,
				{
					{Cam.GetBuff(),CBV},
					{mesh.material->buff,CBV},
					{m_worldMatriciesBuff[enemyType],SRV},
					{m_boneMatriciesBuff[enemyType],SRV},
					{AttachCubeMap->GetCubeMapTex(),SRV},
					{mesh.material->texBuff[COLOR_TEX],SRV},
					{mesh.material->texBuff[METALNESS_TEX],SRV},
					{mesh.material->texBuff[ROUGHNESS_TEX],SRV},
				},
				0.0f,
				false,
				enemyNum
				);
		}
	}
}

void EnemyManager::Spawn(const ENEMY_TYPE& Type, const Transform& InitTransform)
{
	//生成、配列に追加
	auto newEnemy = m_breeds[Type]->Generate();
	newEnemy->Init();
	m_enemys[Type].push_back(newEnemy);

	//上限を超えていたらエラー
	assert(m_enemys[Type].size() <= s_maxNum);

	//ワールド行列配列構造化バッファ生成
	if (!m_worldMatriciesBuff[Type])
	{
		std::array<Matrix, s_maxNum>initMat;
		initMat.fill(XMMatrixIdentity());
		m_worldMatriciesBuff[Type] = D3D12App::Instance()->GenerateStructuredBuffer(sizeof(Matrix), s_maxNum, initMat.data(), "Enemy's world matricies");
	}

	//ボーン行列配列構造化バッファ生成
	const int boneNum = static_cast<int>(m_breeds[Type]->GetModel()->m_skelton->bones.size());
	if (boneNum && !m_boneMatriciesBuff[Type])
	{
		//ボーンの最大数を超えていたらエラー
		assert(boneNum < s_maxBoneNum);

		std::vector<Matrix>initMat(s_maxBoneNum * s_maxNum);
		std::fill(initMat.begin(), initMat.end(), XMMatrixIdentity());
		m_boneMatriciesBuff[Type] = D3D12App::Instance()->GenerateStructuredBuffer(sizeof(Matrix) * s_maxBoneNum, s_maxNum, initMat.data(), "Enemy's bone matricies");
	}
}


