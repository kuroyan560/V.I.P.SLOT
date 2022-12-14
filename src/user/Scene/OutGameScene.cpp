#include "OutGameScene.h"

void OutGameScene::OnInitialize()
{
}

void OutGameScene::OnUpdate()
{
	//入力取得
	auto input = UsersInput::Instance();
	//決定入力
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	//決定
	if (enter)
	{
		KuroEngine::Instance()->ChangeScene("InGame");
	}
}

void OutGameScene::OnDraw()
{
}

void OutGameScene::OnImguiDebug()
{
	ImGui::Begin("OutGameScene");
	ImGui::Text("Press A");
	ImGui::End();
}

void OutGameScene::OnFinalize()
{
}
