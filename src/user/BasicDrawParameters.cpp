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
		ImGui::ColorPicker4("BrightMulColor", (float*)&m_brightMulColor);
		//暗い部分に乗算する色
		ImGui::ColorPicker4("DarkMulColor", (float*)&m_darkMulColor);
		//エッジカラー
		ImGui::ColorPicker4("EdgeColor", (float*)&m_edgeColor);
		ImGui::TreePop();
	}
}
