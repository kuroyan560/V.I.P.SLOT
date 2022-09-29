#include "EnemyManager.h"
#include"EnemyBreed.h"
#include"Importer.h"
#include"EnemyController.h"
#include"Enemy.h"

EnemyManager::EnemyManager()
{
	/*--- 敵の定義 ---*/

	//横移動する雑魚敵
	{
		int weakSlideIdx = static_cast<int>(ENEMY_TYPE::WEAK_SLIDE);
		m_breeds[weakSlideIdx] = std::make_shared<EnemyBreed>(
			weakSlideIdx,
			Importer::Instance()->LoadModel("resource/user/model/", "enemy_test.glb"),
			1,
			10,
			std::make_unique<EnemySlideMove>(0.1f)
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
}

void EnemyManager::Init()
{
	for (int typeIdx = 0; typeIdx < static_cast<int>(ENEMY_TYPE::NUM); ++typeIdx)
	{
		//生存エネミー配列を空に
		m_aliveEnemyArray[typeIdx].clear();

		//死亡エネミー配列にエネミー配列コピー
		m_deadEnemyArray[typeIdx] = m_enemys[typeIdx];
	}
}

void EnemyManager::Update(const TimeScale& arg_timeScale)
{
	for (auto& aliveEnemys : m_aliveEnemyArray)
	{
		for (auto& enemy : aliveEnemys)
		{
			enemy->Update(arg_timeScale);
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

void EnemyManager::Appear(ENEMY_TYPE arg_type)
{
	//敵種別番号取得
	int typeIdx = static_cast<int>(arg_type);

	//新規追加する敵取得
	auto& newEnemy = m_deadEnemyArray[typeIdx].front();

	//生存エネミー配列に追加
	m_aliveEnemyArray[typeIdx].push_front(newEnemy);

	//新規敵の初期化
	newEnemy->Init();

	//新規敵を死亡エネミー配列からポップ
	m_deadEnemyArray[typeIdx].pop_front();
}
