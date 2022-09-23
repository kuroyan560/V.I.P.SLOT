#include "GameScene.h"
#include"GameManager.h"
#include"Collider.h"

GameScene::GameScene()
{

}

void GameScene::OnInitialize()
{
}

void GameScene::OnUpdate()
{
	if (UsersInput::Instance()->KeyOnTrigger(DIK_I))
	{
		this->Initialize();
	}
}


void GameScene::OnDraw()
{
	auto nowCam = GameManager::Instance()->GetNowCamera();

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