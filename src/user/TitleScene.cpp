#include "TitleScene.h"

TitleScene::TitleScene()
{
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
