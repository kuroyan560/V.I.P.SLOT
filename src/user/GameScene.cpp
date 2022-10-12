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
	//コライダー振る舞いリストセット
	m_collisionMgr->AddAttribute("Player", 0b00000001);

	int bitOffset = 1;
	for (int objType = 0; objType < static_cast<int>(ConstParameter::GameObject::TYPE::NUM); ++objType)
	{
		m_collisionMgr->AddAttribute(ConstParameter::GameObject::COLLIDER_ATTRIBUTE[objType], 0b00000001 << bitOffset);
		++bitOffset;
	}

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

	//敵マネージャ生成
	m_enemyMgr = std::make_shared<ObjectManager>();

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

	//敵マネージャ初期化
	m_enemyMgr->Init(m_collisionMgr);

	m_emitEnemyTimer.Reset(m_emitEnemySpan);

	//ステージマネージャ
	m_stageMgr->Init("");
}

void GameScene::OnUpdate()
{
	//デバッグ用
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}
	//敵出現
	if (m_emitEnemyTimer.UpdateTimer(m_timeScale.GetTimeScale()))
	{
		m_enemyMgr->Appear(ConstParameter::GameObject::TYPE::WEAK_SLIDE, m_collisionMgr);
		m_emitEnemyTimer.Reset();
	}

	//コリジョンマネージャ
	m_collisionMgr->Update();

	//カメラ
	m_gameCam->Update(m_timeScale.GetTimeScale());

	//プレイヤー
	m_player->Update(m_slotMachine, m_timeScale);

	//スロットマシン
	m_slotMachine->Update(m_player, m_timeScale);

	//敵マネージャ
	m_enemyMgr->Update(m_timeScale, m_collisionMgr, m_player);

	//ステージマネージャ
	m_stageMgr->Update();
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

	//敵
	m_enemyMgr->Draw(m_ligMgr, m_gameCam->GetFrontCam());
	
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
}

void GameScene::OnImguiDebug()
{
	ImGui::Begin("Debug");
	if (ImGui::DragInt("WeakEnemyEmitSpan", &m_emitEnemySpan))
	{
		if (m_emitEnemySpan < 1)m_emitEnemySpan = 1;
		m_emitEnemyTimer.Reset(m_emitEnemySpan);
	}
	ImGui::End();

	m_stageMgr->ImguiDebug();
}

void GameScene::OnFinalize()
{
}