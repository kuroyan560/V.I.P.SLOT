#include "GameManager.h"
#include"imguiApp.h"
#include"Player.h"

GameManager::GameManager()
{
	RegisterCamera(m_debugCamKey, m_debugCam);
}

void GameManager::Update()
{
	//�f�o�b�O�J�����̍X�V
	if (!m_debugCamKey.compare(m_nowCamKey))
	{
		m_debugCam.Move();
	}
}

void GameManager::ImGuiDebug(UsersInput& Input)
{
	m_controllerConfig.ImguiDebug(Input);

	ImGui::Begin("GameManager");

/*--- �f�o�b�O�`�� ---*/
	ImGui::Checkbox("DebugDraw", &m_debugDraw);

	ImGui::Separator();

/*--- �� ---*/
	//�d��
	ImGui::InputFloat("Gravity", &m_gravity);

	ImGui::Separator();


/*--- �J�����I�� ---*/
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 100), ImGuiWindowFlags_NoTitleBar);
	for (auto& cam : m_cameras)
	{
		//�J�����L�[�̎擾
		const auto& camKey = cam.first;	
		if (ImGui::RadioButton(camKey.c_str(), m_nowCamKey == camKey))	//�I�����ꂽ��
		{
			//�J�����ύX
			ChangeCamera(camKey);
		}
	}
	ImGui::EndChild();

	ImGui::End();
}
