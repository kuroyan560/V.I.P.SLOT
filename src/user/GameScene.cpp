#include "GameScene.h"
#include"GameManager.h"
#include"Collider.h"
#include"Player.h"
#include"RenderTargetManager.h"
#include"LightManager.h"

GameScene::GameScene()
{
	//プレイヤー生成
	m_player = std::make_shared<Player>();

	//ライトマネージャ生成
	m_ligMgr = std::make_shared<LightManager>();
}

void GameScene::OnInitialize()
{
	//プレイヤー初期化
	m_player->Init();
}

void GameScene::OnUpdate()
{
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}

	//ゲームマネージャ更新
	GameManager::Instance()->Update();

	//プレイヤー更新
	m_player->Update();
}


void GameScene::OnDraw()
{
	auto nowCam = GameManager::Instance()->GetNowCamera();
	auto& rtMgr = *RenderTargetManager::Instance();

	rtMgr.Clear();
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });
	m_player->Draw(m_ligMgr, nowCam);

	//デバッグ描画
#ifdef _DEBUG
	if (GameManager::Instance()->GetDebugDrawFlg())
	{
		Collider::DebugDrawAllColliders(*nowCam);
	}
#endif
}

void GameScene::OnImguiDebug()
{
	GameManager::Instance()->ImGuiDebug(*UsersInput::Instance());
}

void GameScene::OnFinalize()
{
}