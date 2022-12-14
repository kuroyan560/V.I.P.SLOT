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
#include"InGameMonitor.h"
#include"ParticleMgr2D.h"
#include"Screen.h"

void InGameScene::OnImguiItems()
{
	ImGui::Text("TimeScale : { %.6f }", m_timeScale.GetTimeScale());
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

	//背景画像読み込み
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//ウェーブマネージャ生成
	m_waveMgr = std::make_shared<WaveMgr>();

	//ステージマネージャ生成
	m_stageMgr = std::make_shared<StageMgr>(m_slotMachine);

	//エネミーエミッター生成
	m_enemyEmitter = std::make_shared<EnemyEmitter>();

	//スクリーン生成
	m_screen = std::make_shared<Screen>();

	//イベント生成
	m_startWaveEvent = std::make_shared<StartWave>();
	m_clearWaveEvent = std::make_shared<ClearWave>();

	//オブジェクト挙動操作クラスにオブジェクトマネージャを渡す
	ObjectController::AttachObjectManager(m_objMgr);
	
	//ブロックヒットSE
	int blockBrokenSE = AudioApp::Instance()->LoadAudio("resource/user/sound/block_broken.wav", 0.5f);

	//ブロックの静的パラメータ設定
	Block::StaticAwake(blockBrokenSE);

	//エネミーエミッターにオブジェクトマネージャとコリジョンマネージャを渡す
	m_enemyEmitter->Awake(m_objMgr, m_collisionMgr);

	//参照用のポインタ窓口クラスにゲーム内情報のポインタセット
	InGameMonitor::Set(m_player.get());

	//ウェーブスタート時イベントにポインタを渡す
	m_startWaveEvent->Awake(
		m_waveMgr,
		m_screen,
		m_gameCam);

	//ウェーブクリア時イベントにポインタを渡す
	m_clearWaveEvent->Awake(
		m_gameCam,
		m_waveMgr,
		m_enemyEmitter,
		m_player,
		m_objMgr,
		m_collisionMgr,
		m_startWaveEvent,
		&m_timeScale);

	//ステージマネージャにコリジョンマネージャを渡す
	m_stageMgr->Awake(m_collisionMgr);
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
	m_stageMgr->Init("");

	//オブジェクトマネージャ
	m_objMgr->Init(m_collisionMgr);

	//エネミーエミッター生成
	m_enemyEmitter->Init();

	//照明設定
	m_dirLig.SetDir(Vec3<float>(0.0f, -0.09f, 0.03f));

	//タイムスケールリセット
	m_timeScale.Set(1.0f);

	//ウェーブマネージャ
	std::list<Wave>test;
	test.emplace_back();
	test.back().m_healKitNum = 3;
	test.emplace_back();
	test.back().m_norma = 30;
	test.back().m_healKitNum = 0;
	m_waveMgr->Init(test);

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
		m_startWaveEvent.get(),
		m_clearWaveEvent.get(),
		this });

	//HUD描画フラグ
	HUDInterface::s_draw = true;

	//パーティクルマネージャ2D
	ParticleMgr2D::Instance()->Init();

	//ウェーブ開始イベント
	m_startWaveEvent->SetNextEvent();
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
	if (UsersInput::Instance()->KeyOnTrigger(DIK_S)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::LB))
	{
		m_slotMachine->Booking();
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_UP)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_UP))
	{
		m_player->Heal(1);
	}
	if (UsersInput::Instance()->KeyOnTrigger(DIK_DOWN)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_DOWN))
	{
		m_player->DebugDamage();
	} 
	if (UsersInput::Instance()->KeyOnTrigger(DIK_H)
		|| UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::Y))
	{
		m_startWaveEvent->SetNextEvent();
	}

	//イベント
	Event::StaticUpdate();

	//カメラ
	m_gameCam->Update(m_timeScale.GetTimeScale(),
		m_player->GetCenterPos() - ConstParameter::Player::INIT_POS);

	//プレイヤー
	m_player->Update(m_slotMachine, m_timeScale);

	//スロットマシン
	m_slotMachine->Update(m_timeScale);

	//ステージマネージャ
	m_stageMgr->Update(m_timeScale);

	//ウェーブマネージャ
	if (m_startWaveEvent->GetEmitEnemy())m_waveMgr->Update(m_timeScale, m_enemyEmitter);

	//オブジェクトマネージャ
	m_objMgr->Update(m_timeScale, m_collisionMgr);

	//パーティクルマネージャ2D
	ParticleMgr2D::Instance()->Update(m_timeScale.GetTimeScale());

	//コリジョンマネージャ
	m_collisionMgr->Update();

	//ウェーブクリアしたか
	if (m_waveMgr->IsWaveClear(m_player->GetCoinNum()) && !m_clearWaveEvent->IsProceed())
	{
		m_clearWaveEvent->SetNextEvent();
	}

	//全ウェーブクリアしたか
	if (m_waveMgr->IsAllWaveClear())
	{
		//アウトゲームへ
		KuroEngine::Instance()->ChangeScene("OutGame");
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
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_MAP);

	//レンダーターゲットセット
	rtMgr.Set(true);

	//2D背景
	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	auto mainCam = Event::GetMainEventCamera();
	auto subCam = Event::GetSubEventCamera();
	if (mainCam == nullptr)mainCam = m_gameCam->GetMainCam();
	if (subCam == nullptr)subCam = Event::GetSubEventCamera();

	//スロットマシン
	m_slotMachine->Draw(m_ligMgr, mainCam);

	//大型スクリーン
	m_screen->Draw(m_ligMgr, mainCam);

	//ステージマネージャ（ブロック）
	m_stageMgr->BlockDraw(m_ligMgr, mainCam);

	//デプスステンシルクリア
	//rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);

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
//#ifdef _DEBUG
	if (m_isDrawCollider)
	{
		m_collisionMgr->DebugDraw(*m_gameCam->GetMainCam());
	}
//#endif

	//デプスステンシルクリア
	rtMgr.Clear(DRAW_TARGET_TAG::DEPTH_STENCIL);

	m_player->Draw2D(mainCam);
	m_stageMgr->Draw2D(mainCam);
	m_waveMgr->Draw2D();

	//パーティクルマネージャ2D
	ParticleMgr2D::Instance()->Draw();
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