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
	arg_collisionMgr.lock()->Register("GameObject", arg_obj->m_colliders);
}

void ObjectManager::OnEnemyDead(std::shared_ptr<GameObject>& arg_obj, std::weak_ptr<CollisionManager>arg_collisionMgr, bool arg_dropCoin, const std::weak_ptr<Player>& arg_player)
{
	//コライダー登録解除
	for (auto& col : arg_obj->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}

	//コインを落とす
	if (arg_dropCoin)
	{
		//落としたコインの放出パワー
		static const float DROP_COIN_EMIT_POWER = 0.5f;
		//放出パワーX方向の強さレート下限
		static const float DROP_COIN_EMIT_X_POWER_RATE_MIN = 0.05f;
		//放出パワーX方向の強さレート上限
		static const float DROP_COIN_EMIT_X_POWER_RATE_MAX = 0.4f;
		//放出パワーY方向の強さレート下限
		static const float DROP_COIN_EMIT_Y_POWER_RATE_MIN = 0.8f;
		//放出パワーY方向の強さレート上限
		static const float DROP_COIN_EMIT_Y_POWER_RATE_MAX = 1.0f;

		//放出パワー
		static const float power = DROP_COIN_EMIT_POWER;

		//放出ベクトル
		float vec_y = KuroFunc::GetRand(DROP_COIN_EMIT_Y_POWER_RATE_MIN, DROP_COIN_EMIT_Y_POWER_RATE_MAX);
		float vec_x = KuroFunc::GetRand(DROP_COIN_EMIT_X_POWER_RATE_MIN, DROP_COIN_EMIT_X_POWER_RATE_MAX);
		float vec_z = 0.0f;
		vec_x *= KuroFunc::GetRandPlusMinus();

		//放出移動量
		const Vec3<float>initMove = Vec3<float>(vec_x, vec_y, vec_z) * power;

		m_dropCoinObjManager.Add(
			arg_obj->m_coinVault.GetNum(),
			arg_obj->m_transform,
			new DropCoinPerform(initMove, arg_player));
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
			OnEnemyDead(enemy, arg_collisionMgr, false, std::weak_ptr<Player>());
		}

		//生存エネミー配列を空に
		m_aliveObjectArray[typeIdx].clear();

		//死亡エネミー配列にエネミー配列コピー
		m_deadObjectArray[typeIdx] = m_objects[typeIdx];
	}

	//落としたコインリセット
	m_dropCoinObjManager.Init();

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
				OnEnemyDead(enemy, arg_collisionMgr, enemy->IsKilled(), arg_player);
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

	int dropCoinReturnNum = m_dropCoinObjManager.Update(arg_timeScale.GetTimeScale());
	if (dropCoinReturnNum)
	{
		AudioApp::Instance()->PlayWave(m_dropCoinReturnSE);
		m_dropCoinEffect.Emit(arg_player.lock()->GetCenterPos(), dropCoinReturnNum);
	}
	arg_player.lock()->GetCoin(dropCoinReturnNum);
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

	m_dropCoinObjManager.Draw(arg_lightMgr, arg_cam);

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

ObjectManager::DropCoinPerform::DropCoinPerform(Vec3<float> arg_initMove, const std::weak_ptr<Player> arg_player)
	: m_move(arg_initMove), m_player(arg_player)
{
	m_move.z = 0.0f;
	//プレイヤーに回収される挙動の時間
	const int COLLECT_TOTAL_TIME = 60;
	m_collectTimer.Reset(COLLECT_TOTAL_TIME);
}

void ObjectManager::DropCoinPerform::OnUpdate(Coins& arg_coin, float arg_timeScale)
{
	//プレイヤーのトランスフォームポインタがアタッチされていない
	if (m_player.expired())
	{
		printf("Error : The drop coin hasn't playerTransform's pointer.\n");
		assert(0);
	}

	using namespace ConstParameter::Environment;

	//跳ね返り時の移動量減衰率
	const float MOVE_REFLECT_RATE = -0.1f;

	//接地時のX軸移動量減衰率
	const float MOVE_X_DAMP_RATE_ON_GROUND = 0.9f;

	//コインの座標取得
	auto pos = arg_coin.m_transform.GetPos();

	//自由挙動
	if (!m_collect)
	{
		pos += m_move * arg_timeScale;

		m_move.y += m_fallAccel;
		m_fallAccel += COIN_GRAVITY * arg_timeScale;

		//X方向の移動量は空気抵抗で減衰
		m_move.x = KuroMath::Lerp(m_move.x, 0.0f, 0.01f);

		//押し戻し（床）
		if (pos.y < FIELD_HEIGHT_HALF)
		{
			pos.y = FIELD_HEIGHT_HALF;
			m_fallAccel = 0.0f;
			m_move.x *= MOVE_X_DAMP_RATE_ON_GROUND;
			m_move.y *= MOVE_REFLECT_RATE;
			m_onGroundPos = pos;

			//移動量が一定以下になったらプレイヤーが回収
			const float MOVE_ABSOLUTE_MIN_FOR_COLLECT = 0.05f;
			if (m_move.Length() < MOVE_ABSOLUTE_MIN_FOR_COLLECT)
			{
				m_collect = true;
			}
		}

		//押し戻し（ステージ端）
		if (pos.x < -FIELD_WIDTH_HALF)
		{
			pos.x = -FIELD_WIDTH_HALF;
			m_move.x *= MOVE_REFLECT_RATE;
		}
		else if (FIELD_WIDTH_HALF < pos.x)
		{
			pos.x = FIELD_WIDTH_HALF;
			m_move.x *= MOVE_REFLECT_RATE;
		}
	}
	//プレイヤーに回収される挙動
	else
	{
		//タイマー計測
		m_collectTimer.UpdateTimer(arg_timeScale);
		//プレイヤーのモデル中央座標取得
		auto playerPos = m_player.lock()->GetCenterPos();
		//プレイヤーに向かう
		pos = KuroMath::Ease(In, Quint, m_collectTimer.GetTimeRate(), m_onGroundPos, playerPos);
	}

	//変更後の座標を適用
	arg_coin.m_transform.SetPos(pos);
}

bool ObjectManager::DropCoinPerform::IsDead(Coins& arg_coin)
{
	return m_collectTimer.IsTimeUp();
}
