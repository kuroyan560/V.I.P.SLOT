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
		//�R�C�����o
		coin.Update(arg_timeScale);

		//�����؂�
		if (coin.IsDead())
		{
			//���o�I�������R�C���̖������Z
			finishCoinNum += coin.m_coinNum;
		}
	}

	//���S������폜
	m_coins.remove_if([](Coins& c)
		{
			return c.IsDead();
		});

	return finishCoinNum;
}

#include"DrawFunc3D.h"
void CoinObjectManager::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	//BET���ꂽ�R�C���̕`��
	for (auto& coin : m_coins)
	{
		DrawFunc3D::DrawNonShadingModel(m_coinModel, coin.m_transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
	}
}

void CoinObjectManager::Add(int arg_coinNum, const Transform& arg_initTransform, CoinPerform* arg_perform)
{
	m_coins.emplace_front(arg_coinNum, arg_initTransform, arg_perform);
}