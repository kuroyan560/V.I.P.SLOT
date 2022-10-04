#include "EnemyManager.h"
#include"EnemyBreed.h"
#include"Importer.h"
#include"EnemyController.h"
#include"Enemy.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"Coins.h"
#include"TimeScale.h"
#include"AudioApp.h"

void EnemyManager::OnEnemyAppear(std::shared_ptr<Enemy>& arg_enemy, std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	//新規敵の初期化
	arg_enemy->Init();

	//コライダーの登録
	arg_collisionMgr.lock()->Register("Enemy", arg_enemy->m_colliders);
}

void EnemyManager::OnEnemyDead(std::shared_ptr<Enemy>& arg_enemy, std::weak_ptr<CollisionManager>arg_collisionMgr, bool arg_dropCoin)
{
	//コライダー登録解除
	for (auto& col : arg_enemy->m_colliders)
	{
		arg_collisionMgr.lock()->Remove(col);
	}


	//コインを落とす
	if (arg_dropCoin)
	{
		//落としたコインの放出パワー下限
		const float DROP_COIN_EMIT_POWER_MIN = 0.4f;
		//落としたコインの放出パワー上限
		const float DROP_COIN_EMIT_POWER_MAX = 1.4f;
		//放出パワーX方向の強さレート下限
		const float DROP_COIN_EMIT_X_POWER_RATE_MIN = 0.0f;
		//放出パワーX方向の強さレート上限
		const float DROP_COIN_EMIT_X_POWER_RATE_MAX = 1.0f;
		//放出パワーY方向の強さレート下限
		const float DROP_COIN_EMIT_Y_POWER_RATE_MIN = 0.8f;
		//放出パワーY方向の強さレート上限
		const float DROP_COIN_EMIT_Y_POWER_RATE_MAX = 1.0f;

		//落としたコインの寿命
		const int DROP_COIN_LIFE_TIME = 400;

		//放出パワー
		const float power = KuroFunc::GetRand(DROP_COIN_EMIT_POWER_MIN, DROP_COIN_EMIT_POWER_MAX);

		//放出ベクトル
		float vec_y = KuroFunc::GetRand(DROP_COIN_EMIT_Y_POWER_RATE_MIN, DROP_COIN_EMIT_Y_POWER_RATE_MAX);
		float vec_x = KuroFunc::GetRand(DROP_COIN_EMIT_X_POWER_RATE_MIN, DROP_COIN_EMIT_X_POWER_RATE_MAX);
		float vec_z = 0.0f;
		vec_x *= KuroFunc::GetRandPlusMinus();

		//放出移動量
		const Vec3<float>initMove = Vec3<float>(vec_x, vec_y, vec_z) * power;

		m_dropCoinObjManager.Add(
			arg_enemy->m_coinVault.GetNum(),
			arg_enemy->m_transform,
			DROP_COIN_LIFE_TIME,
			new DropCoinPerform(initMove));
	}
}

EnemyManager::EnemyManager()
{
	/*--- 敵の定義 ---*/

	//横移動する雑魚敵
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>colPrimitiveArray;
		colPrimitiveArray.emplace_back(std::make_shared<CollisionSphere>(2.0f, Vec3<float>(0.0f, 0.0f, 0.0f)));

		std::vector<std::unique_ptr<Collider>>colliderArray;
		colliderArray.emplace_back(std::make_unique<Collider>("Weak_Slide_Enemy - Body_Sphere", colPrimitiveArray));

		int weakSlideIdx = static_cast<int>(ENEMY_TYPE::WEAK_SLIDE);
		m_breeds[weakSlideIdx] = std::make_shared<EnemyBreed>(
			weakSlideIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			5,
			10,
			std::make_unique<EnemySlideMove>(0.1f),
			colliderArray
			);
	}

	/*--- ---*/

	//敵インスタンス生成
	for (int typeIdx = 0; typeIdx < static_cast<int>(ENEMY_TYPE::NUM); ++typeIdx)
	{
		for (int enemyCount = 0; enemyCount < ConstParameter::Enemy::INSTANCE_NUM_MAX[typeIdx]; ++enemyCount)
		{
			m_enemys[typeIdx].emplace_front(std::make_shared<Enemy>(m_breeds[typeIdx]));
		}
	}

	//SE読み込み
	m_dropCoinReturnSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav");
}

void EnemyManager::Init(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	for (int typeIdx = 0; typeIdx < static_cast<int>(ENEMY_TYPE::NUM); ++typeIdx)
	{
		for (auto& enemy : m_aliveEnemyArray[typeIdx])
		{
			OnEnemyDead(enemy, arg_collisionMgr, false);
		}

		//生存エネミー配列を空に
		m_aliveEnemyArray[typeIdx].clear();

		//死亡エネミー配列にエネミー配列コピー
		m_deadEnemyArray[typeIdx] = m_enemys[typeIdx];
	}

	//落としたコインリセット
	m_dropCoinObjManager.Init();
}

void EnemyManager::Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, CoinVault& arg_playerVault)
{
	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		for (auto& enemy : aliveEnemys)
		{
			enemy->Update(arg_timeScale);

			//死んでいたら
			if (enemy->IsDead())
			{
				OnEnemyDead(enemy, arg_collisionMgr, enemy->IsKilled());
			}
		}
	}

	//死亡していたら生存エネミー配列から削除
	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		aliveEnemys.remove_if([](std::shared_ptr<Enemy>& e)
			{
				return e->IsDead();
			});
	}

	int dropCoinReturnNum = m_dropCoinObjManager.Update(arg_timeScale.GetTimeScale());
	if (dropCoinReturnNum)
	{
		AudioApp::Instance()->PlayWave(m_dropCoinReturnSE);
	}
	arg_playerVault.Add(dropCoinReturnNum);
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

	m_dropCoinObjManager.Draw(arg_lightMgr, arg_cam);
}

void EnemyManager::Appear(ENEMY_TYPE arg_type, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//敵種別番号取得
	int typeIdx = static_cast<int>(arg_type);

	//新規追加する敵取得
	auto& newEnemy = m_deadEnemyArray[typeIdx].front();

	//生存エネミー配列に追加
	m_aliveEnemyArray[typeIdx].push_front(newEnemy);

	OnEnemyAppear(newEnemy, arg_collisionMgr);

	//新規敵を死亡エネミー配列からポップ
	m_deadEnemyArray[typeIdx].pop_front();
}

void EnemyManager::DropCoinPerform::OnUpdate(Coins& arg_coin, float arg_timeScale)
{
	using namespace ConstParameter::Environment;

	//跳ね返り時の移動量減衰率
	const float MOVE_REFLECT_RATE = -0.1f;

	//接地時のX軸移動量減衰率
	const float MOVE_X_DAMP_RATE_ON_GROUND = 0.9f;

	auto pos = arg_coin.m_transform.GetPos();
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
	arg_coin.m_transform.SetPos(pos);
}
