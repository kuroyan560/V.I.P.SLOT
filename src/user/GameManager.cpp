#include "GameManager.h"
#include"ConstParameters.h"

void GameManager::FlowStart()
{
	m_coinNum = 0;
	m_playerHp = ConstParameter::Player::MAX_HP;
}