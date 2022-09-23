#include"TerrianManager.h"
#include"Importer.h"
#include"Collider.h"
#include"ActPoint.h"
#include"Model.h"
#include"BasicDraw.h"

TerrianManager::TerrianManager()
{
/*--- 地形パーツ型オブジェクトの定義 ---*/
	//SINGLE_ACT_POINT
	{
		//モデル
		auto model = Importer::Instance()->LoadModel("resource/user/model/", "ActPoint.glb");

		//コライダー
		const float COLLIDER_RADIUS = 1.0f;
		auto colSphere = std::make_shared<CollisionSphere>(COLLIDER_RADIUS, Vec3<float>(0, 0, 0), nullptr);
		auto col = Collider::Generate(
			"SingleActPoint's Terrian Collider",
			colSphere,
			nullptr,
			COLLIDER_ATTRIBUTE::TERRIAN,
			COLLIDER_ATTRIBUTE::PLAYER | COLLIDER_ATTRIBUTE::ENEMY);

		//ActPoint
		auto actPt = ActPoint::Generate(Transform(), false, true);

		//型オブジェクト生成（挙動はnullptr）
		m_stagePartsTypeArray[SINGLE_ACT_POINT] =
			std::unique_ptr<TerrianPartsType>(new TerrianPartsType(model, { col }, nullptr, { actPt }));
	}


	//地形モデルではスキニングアニメーションを使用しない（※当面の方針）
	for (auto& type : m_stagePartsTypeArray)
	{
		assert(type->m_model->m_skelton->animations.empty());
	}
}

void TerrianManager::Spawn(const TERRIAN_PARTS& PartsType, const Transform& InitTransform)
{
	//地形の生成、配列に追加
	m_terrians.push_front(m_stagePartsTypeArray[PartsType]->GenerateParts(InitTransform));
}

void TerrianManager::Clear()
{
	m_terrians.clear();
}

void TerrianManager::Init()
{
	for (auto& terrian : m_terrians)
	{
		terrian->Init();
	}
}

void TerrianManager::Update()
{
	for (auto& terrian : m_terrians)
	{
		terrian->Update();
	}
}

void TerrianManager::Draw(LightManager& LigManager, Camera& Cam, std::shared_ptr<CubeMap>AttachCubeMap)
{
	for (auto& terrian : m_terrians)
	{
		BasicDraw::Draw(LigManager, terrian->m_typeObj->m_model, terrian->m_transform, Cam, AttachCubeMap);
	}
}
