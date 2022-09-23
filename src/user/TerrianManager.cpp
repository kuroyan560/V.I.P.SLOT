#include"TerrianManager.h"
#include"Importer.h"
#include"Collider.h"
#include"ActPoint.h"
#include"Model.h"
#include"BasicDraw.h"

TerrianManager::TerrianManager()
{
/*--- �n�`�p�[�c�^�I�u�W�F�N�g�̒�` ---*/
	//SINGLE_ACT_POINT
	{
		//���f��
		auto model = Importer::Instance()->LoadModel("resource/user/model/", "ActPoint.glb");

		//�R���C�_�[
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

		//�^�I�u�W�F�N�g�����i������nullptr�j
		m_stagePartsTypeArray[SINGLE_ACT_POINT] =
			std::unique_ptr<TerrianPartsType>(new TerrianPartsType(model, { col }, nullptr, { actPt }));
	}


	//�n�`���f���ł̓X�L�j���O�A�j���[�V�������g�p���Ȃ��i�����ʂ̕��j�j
	for (auto& type : m_stagePartsTypeArray)
	{
		assert(type->m_model->m_skelton->animations.empty());
	}
}

void TerrianManager::Spawn(const TERRIAN_PARTS& PartsType, const Transform& InitTransform)
{
	//�n�`�̐����A�z��ɒǉ�
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
