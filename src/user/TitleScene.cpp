#include "TitleScene.h"
#include<magic_enum.h>
#include"GameManager.h"

TitleScene::TitleScene()
{
}

void TitleScene::OnInitialize()
{
	m_item = GAME_START;
}

void TitleScene::OnUpdate()
{
	//デバッグ用
	KuroEngine::Instance()->ChangeScene(1, m_sceneTrans);

	//入力取得
	auto input = UsersInput::Instance();
	//上入力
	bool up = input->ControllerOnTrigger(0, XBOX_STICK::L_UP)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_UP);
	//下入力
	bool down = input->ControllerOnTrigger(0, XBOX_STICK::L_DOWN)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_DOWN);
	//決定入力
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	//項目下へ
	if (m_item < NUM - 1 && down)m_item = (ITEM)(m_item + 1);
	//項目上へ
	else if (0 < m_item && up)m_item = (ITEM)(m_item - 1);

	//決定
	if (enter)
	{
		switch (m_item)
		{
		case GAME_START:
			KuroEngine::Instance()->ChangeScene(1, m_sceneTrans);
			GameManager::Instance()->FlowStart();
			break;
		case EXIT:
			KuroEngine::Instance()->GameEnd();
			break;
		}
	}
}

void TitleScene::OnDraw()
{
}

void TitleScene::OnImguiDebug()
{
	ImGui::Begin("Items");
	ImGui::Text("Now : %s", std::string(magic_enum::enum_name(m_item)).c_str());
	ImGui::End();
}

void TitleScene::OnFinalize()
{
}
