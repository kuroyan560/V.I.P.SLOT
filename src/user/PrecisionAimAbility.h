#pragma once
#include"Ability.h"
//�����G�C��
class PrecisionAimAbility : public Ability
{
	int m_testTimer = 0;
	int m_totalTime = 60;
public:
	PrecisionAimAbility() :Ability("precisionAim.png") {}
	void Init() 
	{
		m_testTimer = 0;
	}
	void Update()
	{
		//��������
		if (!Ability::GetInvokeNotify())return;

		m_testTimer++;

		//�A�r���e�B�I��
		if (m_totalTime <= m_testTimer)
		{
			Init();
			Ability::Finish();
		}
	}
	void Draw();
};

