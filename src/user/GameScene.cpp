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

GameScene::GameScene()
{
	//プレイヤー生成
	m_player = std::make_shared<Player>();

	//ライトマネージャ生成
	m_ligMgr = std::make_shared<LightManager>();

	//床生成
	m_squareFloorObj = std::make_shared<ModelObject>("resource/user/model/", "floor_square.glb");
	m_squareFloorObj->m_transform.SetPos(ConstParameter::Player::INIT_POS);	//プレイヤー初期位置に合わせる
	m_squareFloorObj->GetTransformBuff();	//マッピングのためバッファ呼び出し

	//スロットマシン生成
	m_slotMachineObj = std::make_shared<ModelObject>("resource/user/model/", "slotMachine.glb");

	//背景画像読み込み
	m_backGround = D3D12App::Instance()->GenerateTextureBuffer("resource/user/img/backGround.png");

	//ゲームカメラ生成
	m_gameCam = std::make_shared<GameCamera>();
}

void GameScene::OnInitialize()
{
	//プレイヤー初期化
	m_player->Init();

	//カメラ初期化
	m_gameCam->Init();
}

void GameScene::OnUpdate()
{
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}

	//カメラ更新
	m_gameCam->Update();

	//プレイヤー更新
	m_player->Update();
}


void GameScene::OnDraw()
{
	auto& rtMgr = *RenderTargetManager::Instance();

	rtMgr.Clear();
	rtMgr.Set(true, { DRAW_TARGET_TAG::BACK_BUFF });

	DrawFunc2D::DrawGraph({ 0,0 }, m_backGround, 1.0f, AlphaBlendMode_None);

	DrawFunc3D::DrawNonShadingModel(m_squareFloorObj, *m_gameCam->GetFrontCam(), 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *m_gameCam->GetBackCam(), 1.0f, AlphaBlendMode_None);

	m_player->Draw(m_ligMgr, m_gameCam->GetFrontCam());

	//デバッグ描画
#ifdef _DEBUG
	//Collider::DebugDrawAllColliders(*m_gameCam->GetFrontCam());
#endif
}

void GameScene::OnImguiDebug()
{
}

void GameScene::OnFinalize()
{
}