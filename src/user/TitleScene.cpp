#include "TitleScene.h"
#include"RenderTargetManager.h"

TitleScene::TitleScene()
{
	//レンダーターゲット統括クラス初期化
	RenderTargetManager::Instance()->Init(*D3D12App::Instance());
}

void TitleScene::OnInitialize()
{
}

void TitleScene::OnUpdate()
{
	//とりあえずすぐゲームシーンへ
	KuroEngine::Instance()->ChangeScene(1, m_sceneTrans);
}

void TitleScene::OnDraw()
{
}

void TitleScene::OnImguiDebug()
{
}

void TitleScene::OnFinalize()
{
}
