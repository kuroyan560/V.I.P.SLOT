#include "InGameScene.h"
#include"Collider.h"
#include"Player.h"
#include"RenderTargetManager.h"
#include"LightManager.h"
#include"Object.h"
#include"DrawFunc2D.h"
#include"BasicDraw.h"
#include"GameCamera.h"
#include"ConstParameters.h"
#include"SlotMachine.h"
#include"ObjectManager.h"
#include"CollisionManager.h"
#include"StageMgr.h"
#include"GameManager.h"
#include"ObjectController.h"
#include"EnemyEmitter.h"

InGameScene::InGameScene()
{
	//コリジョンマネージャ生成
	m_collisionMgr = std::make_shared<CollisionManager>();

	//ゲームカメラ生成
	m_gameCam = std::make_shared<GameCamera>();

	//プレイヤー生成
	m_player = std::make_shared<Player>();

	//オブジェクトマネージャ生成
	m_objMgr = std::make_shared<ObjectManager>(m_player->GetNormalAttackCollBack().lock().get());

	//生成時に引数が必要なものの初期化
	m_player->Awake(m_collisionMgr, m_objMgr, m_gameCam);

	//スロットマシン生成
	m_slotMachine = std::make_shared<SlotMachine>();

	//ライトマネージャ生成
	m_ligMgr = std::make_shared<LightManager>();
	m_ligMgr->RegisterDirLight(&m_dirLig);

	//床生成
	m_squareFloorObj = std::make_shared<ModelObject>("resource/user/model/", "floor_square.glb");
	m_squareFloorObj->m_transform.SetPos(ConstParameter::Player::INIT_POS);	//プレイヤー初期位置に合わせる
	m_squareFloorObj->GetTransformBuff();	//マッピングのためバッファ呼び出し

	//背景画像読み込み
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//ステージマネージャ生成
	m_stageMgr = std::make_shared<StageMgr>(m_slotMachine);

	//エネミーエミッター生成
	m_enemyEmitter = std::make_shared<EnemyEmitter>();

	//オブジェクト挙動操作クラスにオブジェクトマネージャを渡す
	ObjectController::AttachObjectManager(m_objMgr);
	
	//ブロックヒットSE
	int blockBrokenSE = AudioApp::Instance()->LoadAudio("resource/user/sound/block_broken.wav", 0.5f);

	//ブロックの静的パラメータ設定
	Block::StaticAwake(blockBrokenSE);
}

void InGameScene::OnInitialize()
{
	auto gameMgr = GameManager::Instance();

	//プレイヤー初期化
	//m_player->Init(gameMgr->GetPlayerHp(), gameMgr->GetCoinNum());
	m_player->Init(gameMgr->GetPlayerHp(), gameMgr->GetCoinNum());

	//スロットマシン初期化
	m_slotMachine->Init();

	//カメラ初期化
	m_gameCam->Init();

	//ステージマネージャ
	m_stageMgr->Init("",m_collisionMgr);

	//オブジェクトマネージャ
	m_objMgr->Init(m_collisionMgr);

	//エネミーエミッター生成
	m_enemyEmitter->Init(m_objMgr, m_collisionMgr);

	//照明設定
	m_dirLig.SetDir(Vec3<float>(0.0f, -0.09f, 0.03f));
}

void InGameScene::OnUpdate()
{
	//デバッグ用
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::BACK))
	{
		this->Initialize();
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_S))
	{
		m_slotMachine->Booking();
	}

	//コリジョンマネージャ
	m_collisionMgr->Update();

	//カメラ
	m_gameCam->Update(m_timeScale.GetTimeScale(),
		m_player->GetCenterPos() - ConstParameter::Player::INIT_POS);

	//プレイヤー
	m_player->Update(m_slotMachine, m_timeScale);

	//スロットマシン
	m_slotMachine->Update(m_player, m_timeScale);

	//ステージマネージャ
	m_stageMgr->Update(m_timeScale, m_collisionMgr, m_player);

	//エネミーエミッター生成
	m_enemyEmitter->TestRandEmit(m_timeScale, m_objMgr, m_collisionMgr);

	//オブジェクトマネージャ
	m_objMgr->Update(m_timeScale, m_collisionMgr, m_player);

	//クリアしたか
	if (m_stageMgr->IsClear(m_player->GetCoinNum()))
	{
		//アウトゲームへ
		KuroEngine::Instance()->ChangeScene(2, m_sceneTrans);
	}
}


void InGameScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	//レンダーターゲットクリア
	rtMgr.Clear();

	//レンダーターゲットセット
	rtMgr.Set(true);

	//2D背景
	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	//スロットマシン
	//m_slotMachine->Draw(m_ligMgr, m_gameCam->GetSubCam());
	m_slotMachine->Draw(m_ligMgr, m_gameCam->GetMainCam());

	//ステージマネージャ（ブロック）
	m_stageMgr->BlockDraw(m_ligMgr, m_gameCam->GetMainCam());

	//床
	BasicDraw::Draw(*m_ligMgr, m_squareFloorObj, *m_gameCam->GetMainCam());
	//DrawFunc3D::DrawNonShadingModel(m_squareFloorObj, *m_gameCam->GetMainCam(), 1.0f, AlphaBlendMode_None);

	//デプスステンシルクリア
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);

	//ステージマネージャ（足場）
	m_stageMgr->ScaffoldDraw(m_ligMgr, m_gameCam->GetMainCam());

	//オブジェクトマネージャ
	m_objMgr->Draw(m_ligMgr, m_gameCam->GetMainCam());

	//プレイヤー
	m_player->Draw(m_ligMgr, m_gameCam->GetMainCam());

	//↓↓↓以降、エミッシブマップとデプスマップへの描画をしない↓↓↓
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//エッジの描画
	if (m_isDrawEdge)
	{
		BasicDraw::DrawEdge(rtMgr.GetDepthMap(), rtMgr.GetEdgeColorMap());
	}

	//デバッグ描画
#ifdef _DEBUG
	if (m_isDrawCollider)
	{
		m_collisionMgr->DebugDraw(*m_gameCam->GetMainCam());
	}
#endif

	//デプスステンシルクリア
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);
	//エフェクト描画
	m_player->Draw2D(m_gameCam->GetMainCam());
	m_stageMgr->Draw2D(m_gameCam->GetMainCam());
}

void InGameScene::OnImguiDebug()
{
	ImGui::Begin("InGame");
	ImGui::Checkbox("IsDrawCollider", &m_isDrawCollider);
	ImGui::Checkbox("IsDrawEdge", &m_isDrawEdge);
	ImGui::End();

	//ConstParameter::ImguiDebug();
	//m_stageMgr->ImguiDebug(m_collisionMgr);
	//m_slotMachine->ImguiDebug();
	m_player->ImguiDebug();
	//m_collisionMgr->ImguiDebug();
	//m_enemyEmitter->ImguiDebug();
	m_ligMgr->ImguiDebug();
	BasicDraw::ImguiDebug();
}

void InGameScene::OnFinalize()
{
	auto gameMgr = GameManager::Instance();

	//ゲーム終了時のプレイヤーの状態を記録
	gameMgr->UpdatePlayersInfo(m_player->GetCoinNum(), m_player->GetHp());

	m_stageMgr->Finalize(m_collisionMgr);
}