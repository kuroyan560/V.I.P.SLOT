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
	//寿命時間
	Timer m_timer;
	//死亡フラグ
	bool m_isDead = false;

	Coins(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime, CoinPerform* arg_perform);
	void Update();
};