#pragma once
#include"Ability.h"

//回復アビリティ
class HealAbility : public Ability
{
	int m_healAmount = 10;
public:
	HealAbility() :Ability("heal.png") {}
	void Update(const int& MaxHp, int& Hp)
	{
		//発動中か
		if (!this->GetInvokeNotify())return;

		//既に最大
		if (MaxHp <= Hp)
		{
			Ability::Finish();
			return;
		}

		//回復
		Hp += m_healAmount;
		//超過防止
		if (MaxHp < Hp)Hp = MaxHp;

		//演出処理

		//アビリティ終了
		Ability::Finish();
	}

	void Draw();
};