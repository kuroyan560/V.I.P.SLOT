#pragma once
#include"Transform.h"
#include"Timer.h"
#include<memory>
class CoinPerform;

class Coins
{
	std::unique_ptr<CoinPerform>m_perform;
public:
	//コイン数
	int m_coinNum;
	//初期化時のトランスフォーム
	const Transform m_initTransform;
	//自身のトランスフォーム
	Transform m_transform;

	Coins(int arg_coinNum, const Transform& arg_initTransform, CoinPerform* arg_perform);
	void Update(float arg_timeScale);
	bool IsDead();
};