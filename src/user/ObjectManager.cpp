#include "ObjectManager.h"
#include"ObjectBreed.h"
#include"Importer.h"
#include"ObjectController.h"
#include"GameObject.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"Coins.h"
#include"TimeScale.h"
#include"AudioApp.h"
#include"Player.h"

std::shared_ptr<GameObject> ObjectManager::OnEnemyAppear(int arg_objTypeIdx, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	//�V�K�ǉ�����G�擾
	auto newEnemy = m_deadObjectArray[arg_objTypeIdx].front();

	//�����G�l�~�[�z��ɒǉ�
	m_aliveObjectArray[arg_objTypeIdx].push_front(newEnemy);

	//�R���C�_�[�̓o�^
	arg_collisionMgr.lock()->Register(newEnemy->m_colliders);

	//�V�K�G�����S�G�l�~�[�z�񂩂�|�b�v
	m_deadObjectArray[arg_objTypeIdx].pop_front();
	
	return newEnemy;
}

void ObjectManager::OnEnemyDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr, const std::weak_ptr<Player>& arg_player)
{
	//�R���C�_�[�o�^����
	for (auto& col : arg_obj->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}
}

ObjectManager::ObjectManager()
{
	/*--- �G�̒�` ---*/

	//���ړ�����G���G
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate("Slide_Move_Enemy_0 - Body_Sphere", "Enemy", colPrimitiveArray);

		int objIdx = static_cast<int>(OBJECT_TYPE::SLIDE_ENEMY);
		m_breeds[objIdx] = std::make_shared<ObjectBreed>(
			objIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			5,
			10,
			std::make_unique<OC_SlideMove>(0.1f),
			colliderArray
			);
	}

	//��ђ��ˁ��e�𔭎ˁi�X���C���C��A��ʊO����o����ђ��ˁ��V���b�g�ňړ��j
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate("Slime_Battery_Enemy_0 - Body_Sphere", "Enemy", colPrimitiveArray);

		int objIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY);
		std::array<float, OC_SlimeBattery::STATUS::NUM>intervalTimes = { 60,60,60,60 };

		m_breeds[objIdx] = std::make_shared<ObjectBreed>(
			objIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			5,
			10,
			std::make_unique<OC_SlimeBattery>(intervalTimes.data(), 0.5f),
			colliderArray
			);
	}

	/*--- ---*/

	//�C���X�^���X����
	for (int typeIdx = 0; typeIdx < static_cast<int>(OBJECT_TYPE::NUM); ++typeIdx)
	{
		for (int enemyCount = 0; enemyCount < ConstParameter::GameObject::INSTANCE_NUM_MAX[typeIdx]; ++enemyCount)
		{
			m_objects[typeIdx].emplace_front(std::make_shared<GameObject>(m_breeds[typeIdx]));
		}
	}

	//SE�ǂݍ���
	m_dropCoinReturnSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav");
}

void ObjectManager::Init(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	for (int typeIdx = 0; typeIdx < static_cast<int>(OBJECT_TYPE::NUM); ++typeIdx)
	{
		for (auto& enemy : m_aliveObjectArray[typeIdx])
		{
			OnEnemyDead(enemy, arg_collisionMgr, std::weak_ptr<Player>());
		}

		//�����G�l�~�[�z������
		m_aliveObjectArray[typeIdx].clear();

		//���S�G�l�~�[�z��ɃG�l�~�[�z��R�s�[
		m_deadObjectArray[typeIdx] = m_objects[typeIdx];
	}

	//���Ƃ����R�C�����v���C���[���E�����ۂ̃G�t�F�N�g
	m_dropCoinEffect.Init();
}

void ObjectManager::Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<Player>arg_player)
{
	for (int enemyTypeIdx = 0; enemyTypeIdx < static_cast<int>(OBJECT_TYPE::NUM); ++enemyTypeIdx)
	{
		for (auto& enemy : m_aliveObjectArray[enemyTypeIdx])
		{
			enemy->Update(arg_timeScale);

			//����ł�����
			if (enemy->IsDead())
			{
				OnEnemyDead(enemy, arg_collisionMgr, arg_player);
				//���S�G�z��ɒǉ�
				m_deadObjectArray[enemyTypeIdx].push_front(enemy);
			}
		}
	}

	//���S���Ă����琶���G�l�~�[�z�񂩂�폜
	for (auto& aliveEnemys : m_aliveObjectArray)
	{
		aliveEnemys.remove_if([](std::shared_ptr<GameObject>& obj)
			{
				return obj->IsDead();
			});
	}

	//���Ƃ����R�C�����v���C���[���E�����ۂ̃G�t�F�N�g
	m_dropCoinEffect.Update(arg_timeScale.GetTimeScale());
}

void ObjectManager::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	for (auto& aliveEnemys : m_aliveObjectArray)
	{
		for (auto& enemy : aliveEnemys)
		{
			enemy->Draw(arg_lightMgr, arg_cam);
		}
	}

	//���Ƃ����R�C�����v���C���[���E�����ۂ̃G�t�F�N�g
	m_dropCoinEffect.Draw(arg_lightMgr, arg_cam);
}

void ObjectManager::AppearSlideMoveEnemy(std::weak_ptr<CollisionManager> arg_collisionMgr, float arg_posY)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIDE_ENEMY);

	//�V�K�G�|�b�v
	auto newEnemy = OnEnemyAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_SlideMove*)newEnemy->m_controller.get())->SetPosY(arg_posY);

	//������
	newEnemy->Init();

}

void ObjectManager::AppearSlimeBattery(std::weak_ptr<CollisionManager> arg_collisionMgr, float arg_appearY, float arg_destinationXArray[], size_t arg_arraySize)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY);

	//�V�K�G�|�b�v
	auto newEnemy = OnEnemyAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_SlimeBattery*)newEnemy->m_controller.get())->SetParameters(arg_appearY, arg_destinationXArray, arg_arraySize);

	//������
	newEnemy->Init();
}