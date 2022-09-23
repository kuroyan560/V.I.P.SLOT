#include "LevelParameters.h"
PlayerLevelParameter LevelParameters::s_player;

#include"imguiApp.h"
void LevelParameters::ImguiDebug()
{
	ImGui::Begin("LevelParameters");

	if (ImGui::TreeNode("Player"))
	{
		//�ʏ�U���Η�
		if (ImGui::DragInt("NormalAttackDamage", &s_player.m_normalAttackDamage, 1) && s_player.m_normalAttackDamage < 0)
		{
			s_player.m_normalAttackDamage = 0;
		}
		//���b�V���U���Η͍ŏ�
		if (ImGui::DragInt("RushDamage_Min", &s_player.m_rushDamageMin, 1) && s_player.m_rushDamageMin < 0)
		{
			s_player.m_rushDamageMin = 0;
		}
		//���b�V���U���Η͍ő�
		if (ImGui::DragInt("RushDamage_Max", &s_player.m_rushDamageMax, 1) && s_player.m_rushDamageMax < 0)
		{
			s_player.m_rushDamageMax = 0;
		}
		//�P�����b�V���Η͍ŏ�
		if (ImGui::DragInt("SingleRushDamage_Min", &s_player.m_singleRushDamageMin, 1) && s_player.m_singleRushDamageMin < 0)
		{
			s_player.m_singleRushDamageMin = 0;
		}
		//�P�����b�V���Η͍ő�
		if (ImGui::DragInt("SingleRushDamage_Max", &s_player.m_singleRushDamageMax, 1) && s_player.m_singleRushDamageMax < 0)
		{
			s_player.m_singleRushDamageMax = 0;
		}

		ImGui::TreePop();
	}

	ImGui::End();
}
