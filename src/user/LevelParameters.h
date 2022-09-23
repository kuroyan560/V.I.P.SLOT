#pragma once
struct PlayerLevelParameter
{
	//通常攻撃火力
	int m_normalAttackDamage = 10;
	//ラッシュ攻撃火力
	int m_rushDamageMin = 5;
	int m_rushDamageMax = 100;
	//単発ラッシュ火力
	int m_singleRushDamageMin = 0;
	int m_singleRushDamageMax = 100;
};

class LevelParameters
{
	static PlayerLevelParameter s_player;
public:
	static const PlayerLevelParameter& GetPlayer() { return s_player; }

	static void ImguiDebug();
};

