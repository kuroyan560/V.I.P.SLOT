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

void ObjectManager::OnEnemyAppear(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	//新規敵の初期化
	arg_obj->Init();

	//コライダーの登録
	arg_collisionMgr.lock()->Register(ConstParameter::GameObject::COLLIDER_ATTRIBUTE[arg_obj->m_breed.lock()->m_typeID], arg_obj->m_colliders);
}

void ObjectManager::OnEnemyDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr, const std::weak_ptr<Player>& arg_player)
{
	//コライダー登録解除
	for (auto& col : arg_obj->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}
}

ObjectManager::ObjectManager()
{
	/*--- 敵の定義 ---*/

	//横移動する雑魚敵
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>("Weak_Slide_Enemy - Body_Sphere", colPrimitiveArray));

		int weakSlideIdx = static_cast<int>(OBJECT_TYPE::WEAK_SLIDE);
		m_breeds[weakSlideIdx] = std::make_shared<ObjectBreed>(
			weakSlideIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			5,
			10,
			std::make_unique<ObjectSlideMove>(0.1f),
			colliderArray
			);
	}

	/*--- ---*/

	//敵インスタンス生成
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
			OnEnemyDead(enemy, arg_collisionMgr, std::weak_ptr<Player>());
		}

		//生存エネミー配列を空に
		m_aliveObjectArray[typeIdx].clear();

		//死亡エネミー配列にエネミー配列コピー
		m_deadObjectArray[typeIdx] = m_objects[typeIdx];
	}

	//落としたコインをプレイヤーが拾った際のエフェクト
	m_dropCoinEffect.Init();
}

void ObjectManager::Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<Player>arg_player)
{
	for (int enemyTypeIdx = 0; enemyTypeIdx < static_cast<int>(OBJECT_TYPE::NUM); ++enemyTypeIdx)
	{
		for (auto& enemy : m_aliveObjectArray[enemyTypeIdx])
		{
			enemy->Update(arg_timeScale);

			//死んでいたら
			if (enemy->IsDead())
			{
				OnEnemyDead(enemy, arg_collisionMgr, arg_player);
				//死亡敵配列に追加
				m_deadObjectArray[enemyTypeIdx].push_front(enemy);
			}
		}
	}

	//死亡していたら生存エネミー配列から削除
	for (auto& aliveEnemys : m_aliveObjectArray)
	{
		aliveEnemys.remove_if([](std::shared_ptr<GameObject>& obj)
			{
				return obj->IsDead();
			});
	}

	//落としたコインをプレイヤーが拾った際のエフェクト
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

	//落としたコインをプレイヤーが拾った際のエフェクト
	m_dropCoinEffect.Draw(arg_lightMgr, arg_cam);
}

void ObjectManager::Appear(OBJECT_TYPE arg_type, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//敵種別番号取得
	int typeIdx = static_cast<int>(arg_type);

	//新規追加する敵取得
	auto& newEnemy = m_deadObjectArray[typeIdx].front();

	//生存エネミー配列に追加
	m_aliveObjectArray[typeIdx].push_front(newEnemy);

	OnEnemyAppear(newEnemy, arg_collisionMgr);

	//新規敵を死亡エネミー配列からポップ
	m_deadObjectArray[typeIdx].pop_front();
}