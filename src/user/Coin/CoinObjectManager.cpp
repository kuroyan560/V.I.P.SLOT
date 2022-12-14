#include "CoinObjectManager.h"
#include"Importer.h"
#include"CoinPerform.h"

CoinObjectManager::CoinObjectManager()
{
	m_coinModel = Importer::Instance()->LoadModel("resource/user/model/", "coin.glb");
}

void CoinObjectManager::Init()
{
	m_coins.clear();
}

int CoinObjectManager::Update(float arg_timeScale)
{
	int finishCoinNum = 0;

	for (auto& coin : m_coins)
	{
		//コイン演出
		coin.Update(arg_timeScale);

		//寿命切れ
		if (coin.IsDead())
		{
			//演出終了したコインの枚数加算
			finishCoinNum += coin.m_coinNum;
		}
	}

	//死亡したら削除
	m_coins.remove_if([](Coins& c)
		{
			return c.IsDead();
		});

	return finishCoinNum;
}

#include"DrawFunc3D.h"
void CoinObjectManager::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	//BETされたコインの描画
	for (auto& coin : m_coins)
	{
		DrawFunc3D::DrawNonShadingModel(m_coinModel, coin.m_transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
	}
}

void CoinObjectManager::Add(int arg_coinNum, const Transform& arg_initTransform, CoinPerform* arg_perform)
{
	m_coins.emplace_front(arg_coinNum, arg_initTransform, arg_perform);
}
