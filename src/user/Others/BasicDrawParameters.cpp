#include "BasicDrawParameters.h"
#include"ImguiApp.h"

void IndividualDrawParameter::ImguiDebugItem()
{
	if (ImGui::TreeNode("Toon"))
	{
		if (ImGui::Button("DefaultColor"))
		{
			*this = GetDefault();
		}

		//�h��Ԃ��F
		if (ImGui::TreeNode("Fill"))
		{
			ImGui::ColorPicker4("FillColor", (float*)&m_fillColor, ImGuiColorEditFlags_AlphaBar);
			ImGui::TreePop();
		}

		//���邢�����ɏ�Z����F
		if (ImGui::TreeNode("BrightMul"))
		{
			ImGui::ColorPicker4("BrightMulColor", (float*)&m_brightMulColor, ImGuiColorEditFlags_AlphaBar);
			ImGui::TreePop();
		}

		//�Â������ɏ�Z����F
		if (ImGui::TreeNode("DarkMul"))
		{
			ImGui::ColorPicker4("DarkMulColor", (float*)&m_darkMulColor, ImGuiColorEditFlags_AlphaBar);
			ImGui::TreePop();
		}

		//�������C�g�����̕␳�F
		if (ImGui::TreeNode("LimBright"))
		{
			ImGui::ColorPicker4("LimBrightColor", (float*)&m_limBrightColor, ImGuiColorEditFlags_AlphaBar);
			ImGui::TreePop();
		}

		//�G�b�W�J���[
		if (ImGui::TreeNode("Edge"))
		{
			ImGui::ColorPicker4("EdgeColor", (float*)&m_edgeColor, ImGuiColorEditFlags_AlphaBar);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}
