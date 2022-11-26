#include "GameOverScene.h"
#include"GameManager.h"
#include<magic_enum.h>

void GameOverScene::OnInitialize()
{
	m_item = RETRY;
}

void GameOverScene::OnUpdate()
{
	//ì¸óÕéÊìæ
	auto input = UsersInput::Instance();
	//è„ì¸óÕ
	bool up = input->ControllerOnTrigger(0, XBOX_STICK::L_UP)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_UP);
	//â∫ì¸óÕ
	bool down = input->ControllerOnTrigger(0, XBOX_STICK::L_DOWN)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_DOWN);
	//åàíËì¸óÕ
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	//çÄñ⁄â∫Ç÷
	if (m_item < NUM - 1 && down)m_item = (ITEM)(m_item + 1);
	//çÄñ⁄è„Ç÷
	else if (0 < m_item && up)m_item = (ITEM)(m_item - 1);

	//åàíË
	if (enter)
	{
		switch (m_item)
		{
		case RETRY:
			KuroEngine::Instance()->ChangeScene("InGame", m_sceneTrans);
			GameManager::Instance()->FlowStart();
			break;
		case TITLE:
			KuroEngine::Instance()->ChangeScene("Title", m_sceneTrans);
			break;
		}
	}
}

void GameOverScene::OnDraw()
{
}

void GameOverScene::OnImguiDebug()
{
	ImGui::Begin("Items");
	ImGui::Text("Now : %s", std::string(magic_enum::enum_name(m_item)).c_str());
	ImGui::End();
}

void GameOverScene::OnFinalize()
{
}
