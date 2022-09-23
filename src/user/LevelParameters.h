#pragma once
struct PlayerLevelParameter
{
	//�ʏ�U���Η�
	int m_normalAttackDamage = 10;
	//���b�V���U���Η�
	int m_rushDamageMin = 5;
	int m_rushDamageMax = 100;
	//�P�����b�V���Η�
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

