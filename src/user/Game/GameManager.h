#pragma once
#include<vector>
#include<string>
#include<memory>
#include"Singleton.h"
#include"CoinVault.h"
#include"PlayersAbility.h"

//シーンに依存しない、ゲーム全体を取りまとめるクラス（シーン間でのデータのやり取りなどを請け負う）
class GameManager : public Singleton<GameManager>
{
	friend class Singleton<GameManager>;
	GameManager() {}

	//ステージデータファイル名配列
	std::vector<std::string>m_stageFilePathArray;

	//現在の階数
	int m_nowFloor;

	//現在の所持金
	int m_coinNum;

	//現在のライフ
	int m_playersRemainLife;

	//プレイヤー能力値
	PlayersAbility m_playersAbility;

public:
	void FlowStart();

	const std::string& GetStageFilePath()const { return m_stageFilePathArray[m_nowFloor]; }
	const int& GetCoinNum()const { return m_coinNum; }
	const int& GetPlayersRemainLife()const { return m_playersRemainLife; }

	//所持金・HPの状態を更新
	void UpdatePlayersInfo(int arg_coinNum, int arg_playerLife)
	{
		m_coinNum = arg_coinNum;
		m_playersRemainLife = arg_playerLife;
	}

	//プレイヤーの能力値ゲッタ
	const PlayersAbility& GetPlayersAbility() { return m_playersAbility; }
};