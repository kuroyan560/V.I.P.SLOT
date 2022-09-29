#include"Coins.h"
#include"CoinPerform.h"

Coins::Coins(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime, CoinPerform* arg_perform)
	:m_coinNum(arg_coinNum), m_initTransform(arg_initTransform), m_transform(m_initTransform), m_perform(std::unique_ptr<CoinPerform>(arg_perform))
{
	m_timer.Reset(arg_lifeTime);
	m_perform->OnEmit(*this);
}

void Coins::Update(float arg_timeScale)
{
	//寿命タイマー更新
	m_timer.UpdateTimer(arg_timeScale);

	m_perform->OnUpdate(*this, arg_timeScale);
}