#include "OutGameScene.h"

void OutGameScene::OnInitialize()
{
}

void OutGameScene::OnUpdate()
{
	auto input = UsersInput::Instance();

	//Œˆ’è“ü—Í
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	if (enter)
	{
		KuroEngine::Instance()->ChangeScene(0, m_sceneTrans);
	}
}

void OutGameScene::OnDraw()
{
}

void OutGameScene::OnImguiDebug()
{
	ImGui::Begin("OutGame");
	ImGui::Text("Press A");
	ImGui::End();
}

void OutGameScene::OnFinalize()
{
}
