#include "GameScene.h"
#include"GameManager.h"
#include"Collider.h"
#include"Player.h"
#include"RenderTargetManager.h"
#include"LightManager.h"
#include"Object.h"
#include"DrawFunc3D.h"

GameScene::GameScene()
{
	//プレイヤー生成
	m_player = std::make_shared<Player>();

	//ライトマネージャ生成
	m_ligMgr = std::make_shared<LightManager>();

	//床生成
	m_floorObj = std::make_shared<ModelObject>("resource/user/model/", "floor.glb");

	//スロットマシン生成
	m_slotMachineObj = std::make_shared<ModelObject>("resource/user/model/", "slotMachine.glb");

	//コイン投入口生成
	m_coinPortObj = std::make_shared<ModelObject>("resource/user/model/", "port.glb");
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

	DrawFunc3D::DrawNonShadingModel(m_floorObj, *nowCam, 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *nowCam, 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_coinPortObj, *nowCam, 1.0f, AlphaBlendMode_None);

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