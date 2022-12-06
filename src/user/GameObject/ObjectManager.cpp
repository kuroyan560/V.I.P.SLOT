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
#include"PlayersCallBack.h"

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

void ObjectManager::OnObjectDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//�R���C�_�[�o�^����
	for (auto& col : arg_obj->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}
}

ObjectManager::ObjectManager(CollisionCallBack* arg_playersNormalAttackCallBack)
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

	//��ђ��ˁ��e�𔭎ˁi�X���C���C��A���[�g�w��j
	{
		int objIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY_ROUTE_DEFINED);

		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate(
			"Slime_Battery_Enemy_RouteDefined_0 - Body_Sphere",
			{ COLLIDER_ATTRIBUTE[objIdx] },
			colPrimitiveArray);

		m_breeds[objIdx] = std::make_shared<ObjectBreed>(
			objIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			5,
			10,
			std::make_unique<OC_SlimeBattery_RouteDefined>(),
			colliderArray
			);
	}

	//��ђ��ˁ��e�𔭎ˁi�X���C���C��A�v���C���[�Ǐ]�j
	{
		int objIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY_CHASE_PLAYER);

		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate(
			"Slime_Battery_Enemy_ChasePlayer_0 - Body_Sphere",
			{ COLLIDER_ATTRIBUTE[objIdx] },
			colPrimitiveArray);

		m_breeds[objIdx] = std::make_shared<ObjectBreed>(
			objIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			5,
			10,
			std::make_unique<OC_SlimeBattery_ChasePlayer>(),
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
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>());
		colliderArray.back()->Generate(
			"Parry_Bullet - Sphere", 
			{ COLLIDER_ATTRIBUTE[objIdx] },
			colPrimitiveArray);
		
		for (auto& col : colliderArray)col->SetCallBack("Enemy", arg_playersNormalAttackCallBack);

		m_breeds[objIdx] = std::make_shared<ObjectBreed>(
			objIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "parry_bullet.glb"),
			1,
			1,
			std::make_unique<OC_TargetObjectEaseMove>(In,Quint,20.0f),
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
			OnObjectDead(enemy, arg_collisionMgr);
		}

		//�����I�u�W�F�N�g�z������
		m_aliveObjectArray[typeIdx].clear();

		//���S�I�u�W�F�N�g�z��ɃI�u�W�F�N�g�z��R�s�[
		m_deadObjectArray[typeIdx] = m_objects[typeIdx];
	}

	//���Ƃ����R�C�����v���C���[���E�����ۂ̃G�t�F�N�g
	m_dropCoinEffect.Init();
}

void ObjectManager::Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//�X�V
	for (int objTypeIdx = 0; objTypeIdx < static_cast<int>(OBJECT_TYPE::NUM); ++objTypeIdx)
	{
		for (auto& obj : m_aliveObjectArray[objTypeIdx])
		{
			obj->Update(arg_timeScale, arg_collisionMgr);

			//����ł�����
			if (obj->IsDead())
			{
				OnObjectDead(obj, arg_collisionMgr);
				//���S�G�z��ɒǉ�
				m_deadObjectArray[objTypeIdx].push_front(obj);
			}
		}
	}

	//���S���Ă����琶���G�l�~�[�z�񂩂�폜
	for (auto& aliveObjects : m_aliveObjectArray)
	{
		aliveObjects.remove_if([](std::shared_ptr<GameObject>& obj)
			{
				return obj->IsDead();
			});
	}

	//���Ƃ����R�C�����v���C���[���E�����ۂ̃G�t�F�N�g
	m_dropCoinEffect.Update(arg_timeScale.GetTimeScale());
}

void ObjectManager::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	for (auto& aliveObjects : m_aliveObjectArray)
	{
		for (auto& obj : aliveObjects)
		{
			obj->Draw(arg_lightMgr, arg_cam);
		}
	}

	//���Ƃ����R�C�����v���C���[���E�����ۂ̃G�t�F�N�g
	m_dropCoinEffect.Draw(arg_lightMgr, arg_cam);
}

std::weak_ptr<GameObject> ObjectManager::AppearEnemyBullet(std::weak_ptr<CollisionManager> arg_collisionMgr, GameObject* arg_parentObj, Vec3<float> arg_initPos, Vec2<float> arg_moveDirXY, float arg_speed, bool arg_sinMeandeling, float arg_meandelingInterval, Angle arg_meandelingAngle)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::ENEMY_BULLET);

	//�V�K�G�|�b�v
	auto newBullet = OnObjectAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_DirectionMove*)newBullet->m_controller.get())->SetParameters(
		arg_moveDirXY,
		arg_speed,
		arg_sinMeandeling,
		arg_meandelingInterval,
		arg_meandelingAngle);


	//������
	newBullet->Init(arg_initPos);
	newBullet->SetParentObj(arg_parentObj);

	return newBullet;
}

std::weak_ptr<GameObject> ObjectManager::AppearParryBullet(std::weak_ptr<CollisionManager> arg_collisionMgr, Vec3<float> arg_initPos, GameObject* arg_target)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::PARRY_BULLET);

	//�V�K�G�|�b�v
	auto newBullet = OnObjectAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_TargetObjectEaseMove*)newBullet->m_controller.get())->SetParameters(arg_target);

	//������
	newBullet->Init(arg_initPos);

	return newBullet;
}

std::weak_ptr<GameObject> ObjectManager::AppearSlideMoveEnemy(std::weak_ptr<CollisionManager> arg_collisionMgr, float arg_moveX, Vec3<float>arg_initPos)
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
	initPos.y = arg_initPos.y;

	//�t�B�[���h��Z�ɍ��킹��
	initPos.z = FIELD_FLOOR_POS.z;

	float speed = abs(arg_moveX);

	//�p�����[�^�ݒ�
	((OC_DirectionMove*)newEnemy->m_controller.get())->SetParameters(
		Vec2<float>(arg_moveX / speed, 0.0f),
		speed);

	//������
	newEnemy->Init(initPos);

	return newEnemy;
}

std::weak_ptr<GameObject> ObjectManager::AppearSlimeBatteryRouteDefined(std::weak_ptr<CollisionManager> arg_collisionMgr, Vec3<float>arg_initPos, float arg_destinationXArray[], size_t arg_arraySize)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY_ROUTE_DEFINED);

	//�V�K�G�|�b�v
	auto newEnemy = OnObjectAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_SlimeBattery_RouteDefined*)newEnemy->m_controller.get())->SetRouteX(arg_destinationXArray, arg_arraySize);

	//������
	newEnemy->Init(arg_initPos);

	return newEnemy;
}

std::weak_ptr<GameObject> ObjectManager::AppearSlimeBatteryChasePlayer(std::weak_ptr<CollisionManager> arg_collisionMgr, Vec3<float> arg_initPos, int arg_chaseCount)
{
	//�G��ʔԍ��擾
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY_CHASE_PLAYER);

	//�V�K�G�|�b�v
	auto newEnemy = OnObjectAppear(typeIdx, arg_collisionMgr);

	//�p�����[�^�ݒ�
	((OC_SlimeBattery_ChasePlayer*)newEnemy->m_controller.get())->SetChaseCount(arg_chaseCount);

	//������
	newEnemy->Init(arg_initPos);

	return newEnemy;
}
