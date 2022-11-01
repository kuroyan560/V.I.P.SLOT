#include "OutGameScene.h"
#include"GameManager.h"
#include<magic_enum.h>

void OutGameScene::OnInitialize()
{
	m_item = RETRY;
}

void OutGameScene::OnUpdate()
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
		case RETRY:
			KuroEngine::Instance()->ChangeScene(1, m_sceneTrans);
			GameManager::Instance()->FlowStart();
			break;
		case TITLE:
			KuroEngine::Instance()->ChangeScene(0, m_sceneTrans);
			break;
		}
	}
}

void OutGameScene::OnDraw()
{
}

void OutGameScene::OnImguiDebug()
{
	ImGui::Begin("Items");
	ImGui::Text("Now : %s", std::string(magic_enum::enum_name(m_item)).c_str());
	ImGui::End();
}

void OutGameScene::OnFinalize()
{
}
