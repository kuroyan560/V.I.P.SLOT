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
/*--- �p�C�v���C������ ---*/
	//�p�C�v���C���ݒ�
	static PipelineInitializeOption PIPELINE_OPTION(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�V�F�[�_�[���
	static Shaders SHADERS;
	SHADERS.m_vs = D3D12App::Instance()->CompileShader("resource/user/shaders/Enemy.hlsl", "VSmain", "vs_6_4");
	SHADERS.m_ps = D3D12App::Instance()->CompileShader("resource/user/shaders/Enemy.hlsl", "PSmain", "ps_6_4");

	//���[�g�p�����[�^
	static std::vector<RootParam>ROOT_PARAMETER =
	{
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�J�������o�b�t�@"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_CBV,"�}�e���A����{���o�b�t�@"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�g�����X�t�H�[���o�b�t�@�z��i�\�����o�b�t�@�j"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�{�[���s��o�b�t�@�i�\�����o�b�t�@�j"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "�L���[�u�}�b�v"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�x�[�X�J���[�e�N�X�`��"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"���^���l�X�e�N�X�`��"),
		RootParam(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,"�e��"),
	};

	//�����_�[�^�[�Q�b�g�`�����
	std::vector<RenderTargetInfo>RENDER_TARGET_INFO = { RenderTargetInfo(D3D12App::Instance()->GetBackBuffFormat(), AlphaBlendMode_None) };

	//�p�C�v���C������
	m_pipeline = D3D12App::Instance()->GenerateGraphicsPipeline(PIPELINE_OPTION, SHADERS, ModelMesh::Vertex::GetInputLayout(), ROOT_PARAMETER, RENDER_TARGET_INFO, { WrappedSampler(false, false) });

/*--- �G�l�~�[�^�I�u�W�F�N�g��` ---*/
	{
		//�T���h�o�b�O�i�������Ă��Ȃ��j
		std::string name = "SandBag";

		auto model = Importer::Instance()->LoadModel("resource/user/", "sandbag.glb");
		std::vector<EnemyActPointInfo> actPoints =
		{
			EnemyActPointInfo(10),
		};

		//�R���C�_�[����
		const float COLLIDER_RADIUS = 7.0f;
		auto colSphere = std::make_shared<CollisionSphere>(COLLIDER_RADIUS, Vec3<float>(0, 0, 0));
		auto col = Collider::Generate(
			name + "'s Colldier",
			colSphere,
			EnemyBreed::GetDamageCallBack(),
			COLLIDER_ATTRIBUTE::ENEMY,
			COLLIDER_ATTRIBUTE::PLAYER | COLLIDER_ATTRIBUTE::PLAYER_ATTACK);

		//�����܂ŃN���[�����Ȃ̂Ŕ�A�N�e�B�u�ɂ���
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
	//�����σG�l�~�[�폜
	for (int enemyType = 0; enemyType < ENEMY_TYPE_NUM; ++enemyType)
	{
		EnemyArray& enemyArray = m_enemys[enemyType];
		enemyArray.clear();
	}
}

void EnemyManager::Init()
{
	//�����σG�l�~�[�폜
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

		//���݂��Ȃ��Ȃ�X���[
		if (enemyArray.empty())continue;

		//�X�V
		for (auto& enemy : enemyArray)
		{
			enemy->Update(Player, Gravity);
		}

		//����ł���폜
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
		//�G�z��擾
		EnemyArray& enemyArray = m_enemys[enemyType];

		//���݂��Ȃ��Ȃ�X���[
		if (enemyArray.empty())continue;

		//���f���擾
		auto& model = m_breeds[enemyType]->GetModel();

		//���[���h�s��ލX�V
		static std::vector<Matrix>WORLD_MATRICIES(s_maxNum);	//�s��z��͎g���܂킵
		std::fill(WORLD_MATRICIES.begin(), WORLD_MATRICIES.end(), XMMatrixIdentity());
		for (int enemyIdx = 0; enemyIdx < enemyArray.size(); ++enemyIdx)
		{
			WORLD_MATRICIES[enemyIdx] = enemyArray[enemyIdx]->GetParentMat();
		}
		m_worldMatriciesBuff[enemyType]->Mapping(WORLD_MATRICIES.data());

		//�{�[���s��X�V
		/*
		if (boneMatriciesBuff[enemyType])	//�{�[���֘A���������Ă��邩
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

		const int enemyNum = static_cast<int>(enemyArray.size());	//�C���X�^���X���擾

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
	//�����A�z��ɒǉ�
	auto newEnemy = m_breeds[Type]->Generate();
	newEnemy->Init();
	m_enemys[Type].push_back(newEnemy);

	//����𒴂��Ă�����G���[
	assert(m_enemys[Type].size() <= s_maxNum);

	//���[���h�s��z��\�����o�b�t�@����
	if (!m_worldMatriciesBuff[Type])
	{
		std::array<Matrix, s_maxNum>initMat;
		initMat.fill(XMMatrixIdentity());
		m_worldMatriciesBuff[Type] = D3D12App::Instance()->GenerateStructuredBuffer(sizeof(Matrix), s_maxNum, initMat.data(), "Enemy's world matricies");
	}

	//�{�[���s��z��\�����o�b�t�@����
	const int boneNum = static_cast<int>(m_breeds[Type]->GetModel()->m_skelton->bones.size());
	if (boneNum && !m_boneMatriciesBuff[Type])
	{
		//�{�[���̍ő吔�𒴂��Ă�����G���[
		assert(boneNum < s_maxBoneNum);

		std::vector<Matrix>initMat(s_maxBoneNum * s_maxNum);
		std::fill(initMat.begin(), initMat.end(), XMMatrixIdentity());
		m_boneMatriciesBuff[Type] = D3D12App::Instance()->GenerateStructuredBuffer(sizeof(Matrix) * s_maxBoneNum, s_maxNum, initMat.data(), "Enemy's bone matricies");
	}
}


