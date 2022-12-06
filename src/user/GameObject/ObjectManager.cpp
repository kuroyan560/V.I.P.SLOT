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
	//新規追加する敵取得
	auto newEnemy = m_deadObjectArray[arg_objTypeIdx].front();

	//生存エネミー配列に追加
	m_aliveObjectArray[arg_objTypeIdx].push_front(newEnemy);

	//コライダーの登録
	arg_collisionMgr.lock()->Register(newEnemy->m_colliders);

	//新規敵を死亡エネミー配列からポップ
	m_deadObjectArray[arg_objTypeIdx].pop_front();
	
	return newEnemy;
}

void ObjectManager::OnObjectDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//コライダー登録解除
	for (auto& col : arg_obj->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}
}

ObjectManager::ObjectManager(CollisionCallBack* arg_playersNormalAttackCallBack)
{
	using namespace ConstParameter::GameObject;

	/*--- 敵の定義 ---*/

	//横移動する雑魚敵
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

	//飛び跳ね＆弾を発射（スライム砲台、ルート指定）
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

	//飛び跳ね＆弾を発射（スライム砲台、プレイヤー追従）
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

	//弾
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

	//パリーで跳ね返す弾
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

	//インスタンス生成
	for (int typeIdx = 0; typeIdx < static_cast<int>(OBJECT_TYPE::NUM); ++typeIdx)
	{
		for (int enemyCount = 0; enemyCount < ConstParameter::GameObject::INSTANCE_NUM_MAX[typeIdx]; ++enemyCount)
		{
			m_objects[typeIdx].emplace_front(std::make_shared<GameObject>(m_breeds[typeIdx]));
		}
	}

	//SE読み込み
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

		//生存オブジェクト配列を空に
		m_aliveObjectArray[typeIdx].clear();

		//死亡オブジェクト配列にオブジェクト配列コピー
		m_deadObjectArray[typeIdx] = m_objects[typeIdx];
	}

	//落としたコインをプレイヤーが拾った際のエフェクト
	m_dropCoinEffect.Init();
}

void ObjectManager::Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//更新
	for (int objTypeIdx = 0; objTypeIdx < static_cast<int>(OBJECT_TYPE::NUM); ++objTypeIdx)
	{
		for (auto& obj : m_aliveObjectArray[objTypeIdx])
		{
			obj->Update(arg_timeScale, arg_collisionMgr);

			//死んでいたら
			if (obj->IsDead())
			{
				OnObjectDead(obj, arg_collisionMgr);
				//死亡敵配列に追加
				m_deadObjectArray[objTypeIdx].push_front(obj);
			}
		}
	}

	//死亡していたら生存エネミー配列から削除
	for (auto& aliveObjects : m_aliveObjectArray)
	{
		aliveObjects.remove_if([](std::shared_ptr<GameObject>& obj)
			{
				return obj->IsDead();
			});
	}

	//落としたコインをプレイヤーが拾った際のエフェクト
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

	//落としたコインをプレイヤーが拾った際のエフェクト
	m_dropCoinEffect.Draw(arg_lightMgr, arg_cam);
}

std::weak_ptr<GameObject> ObjectManager::AppearEnemyBullet(std::weak_ptr<CollisionManager> arg_collisionMgr, GameObject* arg_parentObj, Vec3<float> arg_initPos, Vec2<float> arg_moveDirXY, float arg_speed, bool arg_sinMeandeling, float arg_meandelingInterval, Angle arg_meandelingAngle)
{
	//敵種別番号取得
	int typeIdx = static_cast<int>(OBJECT_TYPE::ENEMY_BULLET);

	//新規敵ポップ
	auto newBullet = OnObjectAppear(typeIdx, arg_collisionMgr);

	//パラメータ設定
	((OC_DirectionMove*)newBullet->m_controller.get())->SetParameters(
		arg_moveDirXY,
		arg_speed,
		arg_sinMeandeling,
		arg_meandelingInterval,
		arg_meandelingAngle);


	//初期化
	newBullet->Init(arg_initPos);
	newBullet->SetParentObj(arg_parentObj);

	return newBullet;
}

std::weak_ptr<GameObject> ObjectManager::AppearParryBullet(std::weak_ptr<CollisionManager> arg_collisionMgr, Vec3<float> arg_initPos, GameObject* arg_target)
{
	//敵種別番号取得
	int typeIdx = static_cast<int>(OBJECT_TYPE::PARRY_BULLET);

	//新規敵ポップ
	auto newBullet = OnObjectAppear(typeIdx, arg_collisionMgr);

	//パラメータ設定
	((OC_TargetObjectEaseMove*)newBullet->m_controller.get())->SetParameters(arg_target);

	//初期化
	newBullet->Init(arg_initPos);

	return newBullet;
}

std::weak_ptr<GameObject> ObjectManager::AppearSlideMoveEnemy(std::weak_ptr<CollisionManager> arg_collisionMgr, float arg_moveX, Vec3<float>arg_initPos)
{
	//敵種別番号取得
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIDE_ENEMY);

	//新規敵ポップ
	auto newEnemy = OnObjectAppear(typeIdx, arg_collisionMgr);

	using namespace ConstParameter::GameObject;
	using namespace ConstParameter::Environment;

	//初期化位置
	Vec3<float>initPos;

	//移動スピードによってスタートXを変える
	if (arg_moveX < 0.0f)
	{
		initPos.x = POS_X_ABS;
	}
	else if (0.0f < arg_moveX)
	{
		initPos.x = -POS_X_ABS;
	}

	//高さランダム
	initPos.y = arg_initPos.y;

	//フィールドのZに合わせる
	initPos.z = FIELD_FLOOR_POS.z;

	float speed = abs(arg_moveX);

	//パラメータ設定
	((OC_DirectionMove*)newEnemy->m_controller.get())->SetParameters(
		Vec2<float>(arg_moveX / speed, 0.0f),
		speed);

	//初期化
	newEnemy->Init(initPos);

	return newEnemy;
}

std::weak_ptr<GameObject> ObjectManager::AppearSlimeBatteryRouteDefined(std::weak_ptr<CollisionManager> arg_collisionMgr, Vec3<float>arg_initPos, float arg_destinationXArray[], size_t arg_arraySize)
{
	//敵種別番号取得
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY_ROUTE_DEFINED);

	//新規敵ポップ
	auto newEnemy = OnObjectAppear(typeIdx, arg_collisionMgr);

	//パラメータ設定
	((OC_SlimeBattery_RouteDefined*)newEnemy->m_controller.get())->SetRouteX(arg_destinationXArray, arg_arraySize);

	//初期化
	newEnemy->Init(arg_initPos);

	return newEnemy;
}

std::weak_ptr<GameObject> ObjectManager::AppearSlimeBatteryChasePlayer(std::weak_ptr<CollisionManager> arg_collisionMgr, Vec3<float> arg_initPos, int arg_chaseCount)
{
	//敵種別番号取得
	int typeIdx = static_cast<int>(OBJECT_TYPE::SLIME_BATTERY_ENEMY_CHASE_PLAYER);

	//新規敵ポップ
	auto newEnemy = OnObjectAppear(typeIdx, arg_collisionMgr);

	//パラメータ設定
	((OC_SlimeBattery_ChasePlayer*)newEnemy->m_controller.get())->SetChaseCount(arg_chaseCount);

	//初期化
	newEnemy->Init(arg_initPos);

	return newEnemy;
}
