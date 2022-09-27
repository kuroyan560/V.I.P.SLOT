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
		//�����؂�
		if (coin.m_timer.IsTimeUp())
		{
			//���o�I�������R�C���̖������Z
			finishCoinNum += coin.m_coinNum;
			//���S�t���O�𗧂Ă�
			coin.m_isDead = true;
		}

		//�����^�C�}�[�X�V
		coin.m_timer.UpdateTimer();

		//�R�C�����o
		m_perform->OnUpdate(coin);
	}

	//���S������폜
	m_coins.remove_if([](Coins& c)
		{
			return c.m_isDead;
		});

	return finishCoinNum;
}

#include"DrawFunc3D.h"
void CoinObjectManager::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<GameCamera> arg_gameCam)
{
	//BET���ꂽ�R�C���̕`��
	for (auto& coin : m_coins)
	{
		DrawFunc3D::DrawNonShadingModel(m_coinModel, coin.m_transform, *arg_gameCam.lock()->GetFrontCam(), 1.0f, nullptr, AlphaBlendMode_None);
	}
}

void CoinObjectManager::Add(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime)
{
	m_coins.emplace_front(arg_coinNum, arg_initTransform, arg_lifeTime);
}
