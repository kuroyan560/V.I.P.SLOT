#include "OutGameScene.h"

void OutGameScene::OnInitialize()
{
}

void OutGameScene::OnUpdate()
{
	//���͎擾
	auto input = UsersInput::Instance();
	//�������
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	//����
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
