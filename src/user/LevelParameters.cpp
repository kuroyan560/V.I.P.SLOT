#include "LevelParameters.h"
PlayerLevelParameter LevelParameters::s_player;

#include"imguiApp.h"
void LevelParameters::ImguiDebug()
{
	ImGui::Begin("LevelParameters");

	if (ImGui::TreeNode("Player"))
	{
		//通常攻撃火力
		if (ImGui::DragInt("NormalAttackDamage", &s_player.m_normalAttackDamage, 1) && s_player.m_normalAttackDamage < 0)
		{
			s_player.m_normalAttackDamage = 0;
		}
		//ラッシュ攻撃火力最小
		if (ImGui::DragInt("RushDamage_Min", &s_player.m_rushDamageMin, 1) && s_player.m_rushDamageMin < 0)
		{
			s_player.m_rushDamageMin = 0;
		}
		//ラッシュ攻撃火力最大
		if (ImGui::DragInt("RushDamage_Max", &s_player.m_rushDamageMax, 1) && s_player.m_rushDamageMax < 0)
		{
			s_player.m_rushDamageMax = 0;
		}
		//単発ラッシュ火力最小
		if (ImGui::DragInt("SingleRushDamage_Min", &s_player.m_singleRushDamageMin, 1) && s_player.m_singleRushDamageMin < 0)
		{
			s_player.m_singleRushDamageMin = 0;
		}
		//単発ラッシュ火力最大
		if (ImGui::DragInt("SingleRushDamage_Max", &s_player.m_singleRushDamageMax, 1) && s_player.m_singleRushDamageMax < 0)
		{
			s_player.m_singleRushDamageMax = 0;
		}

		ImGui::TreePop();
	}

	ImGui::End();
}
