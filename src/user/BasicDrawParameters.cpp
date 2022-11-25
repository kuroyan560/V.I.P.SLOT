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

		//明るい部分に乗算する色
		ImGui::ColorPicker4("BrightMulColor", (float*)&m_brightMulColor, ImGuiColorEditFlags_AlphaBar);
		//暗い部分に乗算する色
		ImGui::ColorPicker4("DarkMulColor", (float*)&m_darkMulColor, ImGuiColorEditFlags_AlphaBar);
		//リムライト部分の補正色
		ImGui::ColorPicker4("LimBrightColor", (float*)&m_limBrightColor, ImGuiColorEditFlags_AlphaBar);
		//エッジカラー
		ImGui::ColorPicker4("EdgeColor", (float*)&m_edgeColor, ImGuiColorEditFlags_AlphaBar);
		ImGui::TreePop();
	}
}
