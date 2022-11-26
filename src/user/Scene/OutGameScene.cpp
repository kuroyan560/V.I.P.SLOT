#include "OutGameScene.h"

void OutGameScene::OnInitialize()
{
}

void OutGameScene::OnUpdate()
{
	//“ü—ÍŽæ“¾
	auto input = UsersInput::Instance();
	//Œˆ’è“ü—Í
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	//Œˆ’è
	if (enter)
	{
		KuroEngine::Instance()->ChangeScene("InGame", m_sceneTrans);
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
