#pragma once
#include"Transform.h"
#include"Timer.h"
class Coins
{
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

	Coins(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime)
		:m_coinNum(arg_coinNum), m_initTransform(arg_initTransform), m_transform(m_initTransform)
	{
		m_timer.Reset(arg_lifeTime);
	}
};