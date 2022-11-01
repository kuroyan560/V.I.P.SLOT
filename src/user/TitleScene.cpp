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
	//���͎擾
	auto input = UsersInput::Instance();
	//�����
	bool up = input->ControllerOnTrigger(0, XBOX_STICK::L_UP)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_UP);
	//������
	bool down = input->ControllerOnTrigger(0, XBOX_STICK::L_DOWN)
		|| input->ControllerOnTrigger(0, XBOX_BUTTON::DPAD_DOWN);
	//�������
	bool enter = input->ControllerOnTrigger(0, XBOX_BUTTON::A);

	//���ډ���
	if (m_item < NUM - 1 && down)m_item = (ITEM)(m_item + 1);
	//���ڏ��
	else if (0 < m_item && up)m_item = (ITEM)(m_item - 1);

	//����
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
