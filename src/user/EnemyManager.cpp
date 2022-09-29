#include "EnemyManager.h"
#include"EnemyBreed.h"
#include"Importer.h"
#include"EnemyController.h"
#include"Enemy.h"
#include"Collider.h"
#include"CollisionManager.h"

void EnemyManager::OnEnemyAppear(std::shared_ptr<Enemy>& arg_enemy, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	//�V�K�G�̏�����
	arg_enemy->Init();

	//�R���C�_�[�̓o�^
	for (auto& col : arg_enemy->m_colliders)
	{
		arg_collisionMgr.lock()->Register("Enemy", col);
	}
}

void EnemyManager::OnEnemyDead(std::shared_ptr<Enemy>& arg_enemy, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//�R���C�_�[�o�^����
	for (auto& col : arg_enemy->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}
}

EnemyManager::EnemyManager()
{
	/*--- �G�̒�` ---*/

	//���ړ�����G���G
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(3.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>("Weak_Slide_Enemy - Body_Sphere", colPrimitiveArray));

		int weakSlideIdx = static_cast<int>(ENEMY_TYPE::WEAK_SLIDE);
		m_breeds[weakSlideIdx] = std::make_shared<EnemyBreed>(
			weakSlideIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			1,
			10,
			std::make_unique<EnemySlideMove>(0.1f),
			colliderArray
			);
	}

	/*--- ---*/

	//�G�C���X�^���X����
	for (int typeIdx = 0; typeIdx < static_cast<int>(ENEMY_TYPE::NUM); ++typeIdx)
	{
		for (int enemyCount = 0; enemyCount < ConstParameter::Enemy::INSTANCE_NUM_MAX[typeIdx]; ++enemyCount)
		{
			m_enemys[typeIdx].emplace_front(std::make_shared<Enemy>(m_breeds[typeIdx]));
		}
	}
}

void EnemyManager::Init(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	for (int typeIdx = 0; typeIdx < static_cast<int>(ENEMY_TYPE::NUM); ++typeIdx)
	{
		for (auto& enemy : m_aliveEnemyArray[typeIdx])
		{
			OnEnemyDead(enemy, arg_collisionMgr);
		}

		//�����G�l�~�[�z������
		m_aliveEnemyArray[typeIdx].clear();

		//���S�G�l�~�[�z��ɃG�l�~�[�z��R�s�[
		m_deadEnemyArray[typeIdx] = m_enemys[typeIdx];
	}
}

void EnemyManager::Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		for (auto& enemy : aliveEnemys)
		{
			enemy->Update(arg_timeScale);

			//����ł�����
			if (enemy->IsDead())
			{
				OnEnemyDead(enemy, arg_collisionMgr);
			}
		}
	}

	//���S���Ă����琶���G�l�~�[�z�񂩂�폜
	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		aliveEnemys.remove_if([](std::shared_ptr<Enemy>& e)
			{
				return e->IsDead();
			});
	}
}

void EnemyManager::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		for (auto& enemy : aliveEnemys)
		{
			enemy->Draw(arg_lightMgr, arg_cam);
		}
	}
}

void EnemyManager::Appear(ENEMY_TYPE arg_type, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(arg_type);

	//�V�K�ǉ�����G�擾
	auto& newEnemy = m_deadEnemyArray[typeIdx].front();

	//�����G�l�~�[�z��ɒǉ�
	m_aliveEnemyArray[typeIdx].push_front(newEnemy);

	OnEnemyAppear(newEnemy, arg_collisionMgr);

	//�V�K�G�����S�G�l�~�[�z�񂩂�|�b�v
	m_deadEnemyArray[typeIdx].pop_front();
}
