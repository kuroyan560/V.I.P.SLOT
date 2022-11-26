#include "GameManager.h"
#include"ConstParameters.h"

void GameManager::FlowStart()
{
	m_coinNum = 0;
	m_playerLife = ConstParameter::Player::START_ABILITY_MAX_LIFE;
}
