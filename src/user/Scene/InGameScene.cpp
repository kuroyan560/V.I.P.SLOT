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
#include"WaveMgr.h"
#include"Debugger.h"
#include"Event.h"

void InGameScene::OnImguiItems()
{
	ImGui::Checkbox("IsDrawCollider", &m_isDrawCollider);
	ImGui::Checkbox("IsDrawEdge", &m_isDrawEdge);
}

InGameScene::InGameScene() : Debugger("InGame")
{
	//コリジョンマネージャ生成
	m_collisionMgr = std::make_shared<CollisionManager>();

	//ゲームカメラ生成
	m_gameCam = std::make_shared<GameCamera>();

	//プレイヤー生成
	m_player = std::make_shared<Player>();

	//オブジェクトマネージャ生成
	m_objMgr = std::make_shared<ObjectManager>(m_player->GetCounterAttackCallBack().lock().get());

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

	//ウェーブマネージャ生成
	m_waveMgr = std::make_shared<WaveMgr>();

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
	m_player->Init(gameMgr->GetPlayersAbility(), gameMgr->GetPlayersRemainLife(), gameMgr->GetCoinNum());

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

	//タイムスケールリセット
	m_timeScale.Set(1.0f);

	//ウェーブマネージャ
	m_waveMgr->Init(10);

	//イベント
	Event::StaticInit();

	//デバッガ登録
	Debugger::Register(
		{ m_stageMgr.get(),
		m_waveMgr.get(),
		m_slotMachine.get(),
		m_player.get(),
		m_collisionMgr.get(),
		m_enemyEmitter.get(),
		m_ligMgr.get(),
		BasicDraw::Instance(),
		&m_clearWaveEvent });

	//ウェーブクリア時イベント
	m_clearWaveEvent.Init(m_gameCam, m_player, &m_timeScale);

	//HUD描画フラグ
	HUDInterface::s_draw = true;
}

void InGameScene::OnUpdate()
{
	//デバッグ用
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::BACK))
	{
		this->Finalize();
		this->Initialize();
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_S))
	{
		m_slotMachine->Booking();
	}

	//コリジョンマネージャ
	if (Event::CollisionFlg())m_collisionMgr->Update();

	//カメラ
	m_gameCam->Update(m_timeScale.GetTimeScale(),
		m_player->GetCenterPos() - ConstParameter::Player::INIT_POS);

	//プレイヤー
	m_player->Update(m_slotMachine, m_timeScale);

	//スロットマシン
	m_slotMachine->Update(m_player, m_timeScale);

	//ステージマネージャ
	m_stageMgr->Update(m_timeScale, m_player);

	//エネミーエミッター生成
	m_enemyEmitter->TestRandEmit(m_timeScale, m_objMgr, m_collisionMgr);

	//オブジェクトマネージャ
	m_objMgr->Update(m_timeScale, m_collisionMgr, m_player);

	//イベント
	Event::StaticUpdate();

	//クリアしたか
	if (m_waveMgr->IsClear(m_player->GetCoinNum()))
	{
		HUDInterface::s_draw = false;
		m_clearWaveEvent.Start();
	}

	//死亡したか
	if (m_player->IsDead())
	{
		//ゲームオーバー
		KuroEngine::Instance()->ChangeScene("GameOver");
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

	auto mainCam = Event::GetMainEventCamera();
	auto subCam = Event::GetSubEventCamera();
	if (mainCam == nullptr)mainCam = m_gameCam->GetMainCam();
	if (subCam == nullptr)subCam = Event::GetSubEventCamera();

	//スロットマシン
	//m_slotMachine->Draw(m_ligMgr, m_gameCam->GetSubCam());
	m_slotMachine->Draw(m_ligMgr, mainCam);

	//ステージマネージャ（ブロック）
	m_stageMgr->BlockDraw(m_ligMgr, mainCam);

	//DrawFunc3D::DrawNonShadingModel(m_squareFloorObj, *m_gameCam->GetMainCam(), 1.0f, AlphaBlendMode_None);

	//デプスステンシルクリア
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);

	//床
	BasicDraw::Instance()->Draw(*m_ligMgr, m_squareFloorObj, *mainCam);

	//ステージマネージャ（足場）
	m_stageMgr->ScaffoldDraw(m_ligMgr, mainCam);

	//オブジェクトマネージャ
	m_objMgr->Draw(m_ligMgr, mainCam);

	//プレイヤー
	m_player->Draw(m_ligMgr, mainCam);

	//↓↓↓以降、エミッシブマップとデプスマップへの描画をしない↓↓↓
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	//エッジの描画
	if (m_isDrawEdge)
	{
		BasicDraw::Instance()->DrawEdge(rtMgr.GetDepthMap(), rtMgr.GetEdgeColorMap());
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

	m_player->Draw2D(m_gameCam->GetMainCam());
	m_stageMgr->Draw2D(m_gameCam->GetMainCam());
	m_waveMgr->Draw2D();
}

void InGameScene::OnImguiDebug()
{
	Debugger::Draw();

	ConstParameter::ImguiDebug();
}

void InGameScene::OnFinalize()
{
	auto gameMgr = GameManager::Instance();

	//ゲーム終了時のプレイヤーの状態を記録
	gameMgr->UpdatePlayersInfo(m_player->GetCoinNum(), m_player->GetLife());

	m_stageMgr->Finalize();
	Debugger::ClearRegister();
}