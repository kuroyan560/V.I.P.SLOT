#include "GameManager.h"
#include"ConstParameters.h"

void GameManager::FlowStart()
{
	m_coinNum = 0;
	m_playersRemainLife = ConstParameter::Player::START_ABILITY_MAX_LIFE;
	m_playersAbility.m_maxLife = ConstParameter::Player::START_ABILITY_MAX_LIFE;
	m_playersAbility.m_offensive = ConstParameter::Player::START_ABILITY_OFFSENSIVE;
}
