#include "BasicDrawParameters.h"
#include"ImguiApp.h"

void ToonIndividualParameter::ImguiDebugItem()
{
	if (ImGui::TreeNode("Toon"))
	{
		if (ImGui::Button("DefaultColor"))
		{
			*this = GetDefault();
		}

		//���邢�����ɏ�Z����F
		ImGui::ColorPicker4("BrightMulColor", (float*)&m_brightMulColor);
		//�Â������ɏ�Z����F
		ImGui::ColorPicker4("DarkMulColor", (float*)&m_darkMulColor);
		//�G�b�W�J���[
		ImGui::ColorPicker4("EdgeColor", (float*)&m_edgeColor);
		ImGui::TreePop();
	}
}
