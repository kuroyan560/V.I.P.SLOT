#pragma once
#include"Coins.h"
class CoinPerform
{
public:
	//コインの挙動
	virtual void OnUpdate(Coins& arg_coin, float arg_timeScale) = 0;
	virtual void OnEmit(Coins& arg_coin) = 0;
	virtual bool IsDead(Coins& arg_coin) = 0;
};