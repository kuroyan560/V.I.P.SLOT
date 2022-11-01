#include "TitleScene.h"
#include<magic_enum.h>

TitleScene::TitleScene()
{
}

void TitleScene::OnInitialize()
{
	m_item = GAME_START;
}

void TitleScene::OnUpdate()
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
		case GAME_START:
			KuroEngine::Instance()->ChangeScene(1, m_sceneTrans);
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
