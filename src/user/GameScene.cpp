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
#include"EnemyManager.h"
#include"CollisionManager.h"

GameScene::GameScene()
{
	//コリジョンマネージャ生成
	m_collisionMgr = std::make_shared<CollisionManager>();
	//コライダー振る舞いリストセット
	m_collisionMgr->AddAttribute("Enemy", 0b00000001);
	m_collisionMgr->AddAttribute("Player", 0b00000001 << 1);

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
	m_enemyMgr = std::make_shared<EnemyManager>();
}

void GameScene::OnInitialize()
{
	//プレイヤー初期化
	m_player->Init();

	//スロットマシン初期化
	m_slotMachine->Init();

	//カメラ初期化
	m_gameCam->Init();

	//敵マネージャ初期化
	m_enemyMgr->Init(m_collisionMgr);
}

void GameScene::OnUpdate()
{
	//デバッグ用
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_E))
	{
		m_enemyMgr->Appear(ConstParameter::Enemy::TYPE::WEAK_SLIDE, m_collisionMgr);
	}
	if (UsersInput::Instance()->KeyInput(DIK_T))
	{
		m_timeScale.Set(0.0f);
	}
	else m_timeScale.Set(1.0f);

	//コリジョンマネージャ
	m_collisionMgr->Update();

	//カメラ
	m_gameCam->Update();

	//プレイヤー
	m_player->Update(m_slotMachine, m_timeScale);

	//スロットマシン
	m_slotMachine->Update(m_player->GetVault(), m_timeScale);

	//敵マネージャ
	m_enemyMgr->Update(m_timeScale, m_collisionMgr);
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
	
	//プレイヤー
	m_player->Draw(m_ligMgr, m_gameCam->GetFrontCam());

	//デバッグ描画
#ifdef _DEBUG
	//Collider::DebugDrawAllColliders(*m_gameCam->GetFrontCam());
	m_collisionMgr->DebugDraw(*m_gameCam->GetFrontCam());
#endif
}

void GameScene::OnImguiDebug()
{
}

void GameScene::OnFinalize()
{
}