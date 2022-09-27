#include "CoinObjectManager.h"
#include"Importer.h"
#include"CoinPerform.h"
#include"GameCamera.h"

CoinObjectManager::CoinObjectManager(CoinPerform* arg_coinPerform)
	: m_perform(std::unique_ptr<CoinPerform>(arg_coinPerform))
{
	m_coinModel = Importer::Instance()->LoadModel("resource/user/model/", "coin.glb");
}

void CoinObjectManager::Init()
{
	m_coins.clear();
}

int CoinObjectManager::Update()
{
	int finishCoinNum = 0;

	for (auto& coin : m_coins)
	{
		//寿命切れ
		if (coin.m_timer.IsTimeUp())
		{
			//演出終了したコインの枚数加算
			finishCoinNum += coin.m_coinNum;
			//死亡フラグを立てる
			coin.m_isDead = true;
		}

		//寿命タイマー更新
		coin.m_timer.UpdateTimer();

		//コイン演出
		m_perform->OnUpdate(coin);
	}

	//死亡したら削除
	m_coins.remove_if([](Coins& c)
		{
			return c.m_isDead;
		});

	return finishCoinNum;
}

#include"DrawFunc3D.h"
void CoinObjectManager::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<GameCamera> arg_gameCam)
{
	//BETされたコインの描画
	for (auto& coin : m_coins)
	{
		DrawFunc3D::DrawNonShadingModel(m_coinModel, coin.m_transform, *arg_gameCam.lock()->GetFrontCam(), 1.0f, nullptr, AlphaBlendMode_None);
	}
}

void CoinObjectManager::Add(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime)
{
	m_coins.emplace_front(arg_coinNum, arg_initTransform, arg_lifeTime);
}
