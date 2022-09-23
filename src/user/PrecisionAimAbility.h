#pragma once
#include"Ability.h"
//精密エイム
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
		//発動中か
		if (!Ability::GetInvokeNotify())return;

		m_testTimer++;

		//アビリティ終了
		if (m_totalTime <= m_testTimer)
		{
			Init();
			Ability::Finish();
		}
	}
	void Draw();
};

