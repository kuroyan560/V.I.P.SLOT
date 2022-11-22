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

std::shared_ptr<GameObject> ObjectManager::OnObjectAppear(int arg_objTypeIdx, std::weak_ptr<CollisionManager> arg_collisionMgr)
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

void ObjectManager::OnObjectDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr, const std::weak_ptr<Player>& arg_player)
{
	//�R���C�_�[�o�^����
	for (auto& col : arg_obj->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}
}

ObjectManager::ObjectManager()
{
	using namespace ConstParameter::GameObject;
	/*--- �G�̒�` ---*/

	//���ړ�����G���G
	{
		int objIdx = static_cast<int>(OBJECT_TYPE::SLIDE_ENEMY);
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate(
			"Slide_Move_Enemy_0 - Body_Sphere", 
			{ COLLIDER_ATTRIBUTE[objIdx] }, 
			colPrimitiveArray);

		m_breeds[objIdx] = std::make_shared<ObjectBreed>(
			objIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			5,
			10,
			std::make_unique<OC_DirectionMove>(),
			colliderArray
			);
	}

	//��ђ��ˁ��e�𔭎ˁi�X���C���C��A��ʊO����o����ђ��ˁ��V���b�g�ňړ��j
	{
		int objIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY);

		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate(
			"Slime_Battery_Enemy_0 - Body_Sphere",
			{ COLLIDER_ATTRIBUTE[objIdx] },
			colPrimitiveArray);

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

	//�e
	{
		int objIdx = static_cast<int>(OBJECT_TYPE::ENEMY_BULLET);
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(1.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate(
			"Enemy_Bullet - Sphere", 
			{ COLLIDER_ATTRIBUTE[objIdx] },
			colPrimitiveArray);

		m_breeds[objIdx] = std::make_shared<ObjectBreed>(
			objIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "bullet.glb"),
			1,
			1,
			std::make_unique<OC_DirectionMove>(),
			colliderArray
			);
	}

	//�p���[�Œ��˕Ԃ��e
	{
		int objIdx = static_cast<int>(OBJECT_TYPE::PARRY_BULLET);
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(1.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate(
			"Parry_Bullet - Sphere", 
			{ COLLIDER_ATTRIBUTE[objIdx] },
			colPrimitiveArray);

		m_breeds[objIdx] = std::make_shared<ObjectBreed>(
			objIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "parry_bullet.glb"),
			1,
			1,
			std::make_unique<OC_TargetObjectEaseMove>(In,Back,30.0f),
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
			OnObjectDead(enemy, arg_collisionMgr, std::weak_ptr<Player>());
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
			enemy->Update(arg_timeScale, arg_collisionMgr);

			//����ł�����
			if (enemy->IsDead())
			{
				OnObjectDead(enemy, arg_collisionMgr, arg_player);
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

std::weak_ptr<GameObject> ObjectManager::AppearEnemyBullet(std::weak_ptr<CollisionManager> arg_collisionMgr, GameObject* arg_parentObj, Vec3<float> arg_startPos, Vec2<float> arg_moveDirXY, float arg_speed, bool arg_sinMeandeling, float arg_meandelingInterval, Angle arg_meandelingAngle)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::ENEMY_BULLET);

	//�V�K�G�|�b�v
	auto newBullet = OnObjectAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_DirectionMove*)newBullet->m_controller.get())->SetParameters(
		arg_startPos,
		arg_moveDirXY,
		arg_speed,
		arg_sinMeandeling,
		arg_meandelingInterval,
		arg_meandelingAngle);

	newBullet->SetParentObj(arg_parentObj);

	//������
	newBullet->Init();

	return newBullet;
}

std::weak_ptr<GameObject> ObjectManager::AppearSlideMoveEnemy(std::weak_ptr<CollisionManager> arg_collisionMgr, float arg_moveX, float arg_posY)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIDE_ENEMY);

	//�V�K�G�|�b�v
	auto newEnemy = OnObjectAppear(typeIdx, arg_collisionMgr);

	using namespace ConstParameter::GameObject;
	using namespace ConstParameter::Environment;

	//�������ʒu
	Vec3<float>initPos;

	//�ړ��X�s�[�h�ɂ���ăX�^�[�gX��ς���
	if (arg_moveX < 0.0f)
	{
		initPos.x = POS_X_ABS;
	}
	else if (0.0f < arg_moveX)
	{
		initPos.x = -POS_X_ABS;
	}

	//���������_��
	initPos.y = arg_posY;

	//�t�B�[���h��Z�ɍ��킹��
	initPos.z = FIELD_FLOOR_POS.z;

	float speed = abs(arg_moveX);

	//�p�����[�^�ݒ�
	((OC_DirectionMove*)newEnemy->m_controller.get())->SetParameters(
		initPos,
		Vec2<float>(arg_moveX / speed, 0.0f),
		speed);

	//������
	newEnemy->Init();

	return newEnemy;
}

std::weak_ptr<GameObject> ObjectManager::AppearSlimeBattery(std::weak_ptr<CollisionManager> arg_collisionMgr, float arg_appearY, float arg_destinationXArray[], size_t arg_arraySize)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY);

	//�V�K�G�|�b�v
	auto newEnemy = OnObjectAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_SlimeBattery*)newEnemy->m_controller.get())->SetParameters(arg_appearY, arg_destinationXArray, arg_arraySize);

	//������
	newEnemy->Init();

	return newEnemy;
}

std::weak_ptr<GameObject> ObjectManager::AppearParryBullet(std::weak_ptr<CollisionManager> arg_collisionMgr, Vec3<float> arg_startPos, GameObject* arg_target)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::PARRY_BULLET);

	//�V�K�G�|�b�v
	auto newBullet = OnObjectAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_TargetObjectEaseMove*)newBullet->m_controller.get())->SetParameters(arg_startPos, arg_target);

	//������
	newBullet->Init();

	return newBullet;
}