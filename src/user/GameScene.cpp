#include "GameScene.h"
#include"Collider.h"
#include"Player.h"
#include"RenderTargetManager.h"
#include"LightManager.h"
#include"Object.h"
#include"DrawFunc2D.h"
#include"DrawFunc3D.h"
#include"GameCamera.h"
#include"ConstParameters.h"
#include"SlotMachine.h"
#include"ObjectManager.h"
#include"CollisionManager.h"
#include"StageMgr.h"

GameScene::GameScene()
{
	//コリジョンマネージャ生成
	m_collisionMgr = std::make_shared<CollisionManager>();

	//プレイヤー生成
	m_player = std::make_shared<Player>(m_collisionMgr);

	//スロットマシン生成
	m_slotMachine = std::make_shared<SlotMachine>();

	//ライトマネージャ生成
	m_ligMgr = std::make_shared<LightManager>();

	//床生成
	m_squareFloorObj = std::make_shared<ModelObject>("resource/user/model/", "floor_square.glb");
	m_squareFloorObj->m_transform.SetPos(ConstParameter::Player::INIT_POS);	//プレイヤー初期位置に合わせる
	m_squareFloorObj->GetTransformBuff();	//マッピングのためバッファ呼び出し

	//背景画像読み込み
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//ゲームカメラ生成
	m_gameCam = std::make_shared<GameCamera>();

	//ステージマネージャ生成
	m_stageMgr = std::make_shared<StageMgr>(m_slotMachine);
}

void GameScene::OnInitialize()
{
	//プレイヤー初期化
	m_player->Init(m_gameCam);

	//スロットマシン初期化
	m_slotMachine->Init();

	//カメラ初期化
	m_gameCam->Init();

	//ステージマネージャ
	m_stageMgr->Init("",m_collisionMgr);
}

void GameScene::OnUpdate()
{
	//デバッグ用
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}

	//コリジョンマネージャ
	m_collisionMgr->Update();

	//カメラ
	m_gameCam->Update(m_timeScale.GetTimeScale());

	//プレイヤー
	m_player->Update(m_slotMachine, m_timeScale);

	//スロットマシン
	m_slotMachine->Update(m_player, m_timeScale);

	//ステージマネージャ
	m_stageMgr->Update(m_timeScale,m_collisionMgr);
}


void GameScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	//レンダーターゲットクリア
	rtMgr.Clear();

	//レンダーターゲットセット
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//2D背景
	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	//スロットマシン
	m_slotMachine->Draw(m_ligMgr, m_gameCam);

	//床
	DrawFunc3D::DrawNonShadingModel(m_squareFloorObj, *m_gameCam->GetFrontCam(), 1.0f, AlphaBlendMode_None);

	//ステージマネージャ
	m_stageMgr->Draw(m_ligMgr, m_gameCam->GetFrontCam());

	//プレイヤー
	m_player->Draw(m_ligMgr, m_gameCam->GetFrontCam());

	//デバッグ描画
#ifdef _DEBUG
	//Collider::DebugDrawAllColliders(*m_gameCam->GetFrontCam());
	m_collisionMgr->DebugDraw(*m_gameCam->GetFrontCam());
#endif

	//デプスステンシルクリア
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);
	//エフェクト描画
	m_player->EffectDraw(m_gameCam->GetFrontCam());
	m_stageMgr->EffectDraw(m_gameCam->GetFrontCam());
}

void GameScene::OnImguiDebug()
{
	ConstParameter::ImguiDebug();
	m_stageMgr->ImguiDebug(m_collisionMgr);
	m_slotMachine->ImguiDebug();
}

void GameScene::OnFinalize()
{
}