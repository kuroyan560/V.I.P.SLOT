#pragma once
#include<vector>
#include<string>
#include<memory>
#include"Singleton.h"
#include"CoinVault.h"

class GameManager : public Singleton<GameManager>
{
	friend class Singleton<GameManager>;
	GameManager() {}

	//�X�e�[�W�f�[�^�t�@�C�����z��
	std::vector<std::string>m_stageFilePathArray;

	//���݂̊K��
	int m_nowFloor;

	//���݂̏�����
	int m_coinNum;

	//���݂̃��C�t
	int m_playerLife;

public:
	void FlowStart();

	const std::string& GetStageFilePath()const { return m_stageFilePathArray[m_nowFloor]; }
	const int& GetCoinNum()const { return m_coinNum; }
	const int& GetPlayersRemainLife()const { return m_playerLife; }

	//�������EHP�̏�Ԃ��X�V
	void UpdatePlayersInfo(int arg_coinNum, int arg_playerLife)
	{
		m_coinNum = arg_coinNum;
		m_playerLife = arg_playerLife;
	}
};