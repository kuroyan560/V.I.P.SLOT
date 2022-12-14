#pragma once
#include<stdio.h>
class CoinVault
{
	//所持コイン
	int m_coin = 0;
public:
	//初期化
	void Set(int arg_coinNum = 0) { m_coin = arg_coinNum; }

	//コイン追加
	void Add(int arg_coinNum)
	{
		m_coin += arg_coinNum;
	}

	//全額渡す
	void Pass(CoinVault& arg_dest)
	{
		arg_dest.Add(this->m_coin);
		this->m_coin = 0;

		printf("Pass : (from)%d , (to)%d\n", this->m_coin, arg_dest.m_coin);
	}
	//金額を指定して渡す（足りなければ何もせず、falseを返す）
	bool Pass(CoinVault& arg_dest, int arg_coinNum)
	{
		if (m_coin < arg_coinNum)return false;

		arg_dest.Add(arg_coinNum);
		this->Add(-arg_coinNum);

		printf("Pass : (from)%d , (to)%d\n", this->m_coin, arg_dest.m_coin);

		return true;
	}

	//ゲッタ
	const int& GetNum()const { return m_coin; }
};

