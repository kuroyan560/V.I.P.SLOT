#pragma once
class Coins;
class CoinPerform
{
public:
	//�R�C���̋���
	virtual void OnUpdate(Coins& arg_coin) = 0;
	virtual void OnEmit(Coins& arg_coin) = 0;
};