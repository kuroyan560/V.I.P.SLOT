#include"Coins.h"
#include"CoinPerform.h"

Coins::Coins(int arg_coinNum, const Transform& arg_initTransform, CoinPerform* arg_perform)
	:m_coinNum(arg_coinNum), m_initTransform(arg_initTransform), m_transform(m_initTransform), m_perform(std::unique_ptr<CoinPerform>(arg_perform))
{
	m_perform->OnEmit(*this);
}

void Coins::Update(float arg_timeScale)
{
	m_perform->OnUpdate(*this, arg_timeScale);
}

bool Coins::IsDead()
{
	return m_perform->IsDead(*this);
}
