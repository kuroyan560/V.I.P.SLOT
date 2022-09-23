#pragma once
#include"Ability.h"

//�񕜃A�r���e�B
class HealAbility : public Ability
{
	int m_healAmount = 10;
public:
	HealAbility() :Ability("heal.png") {}
	void Update(const int& MaxHp, int& Hp)
	{
		//��������
		if (!this->GetInvokeNotify())return;

		//���ɍő�
		if (MaxHp <= Hp)
		{
			Ability::Finish();
			return;
		}

		//��
		Hp += m_healAmount;
		//���ߖh�~
		if (MaxHp < Hp)Hp = MaxHp;

		//���o����

		//�A�r���e�B�I��
		Ability::Finish();
	}

	void Draw();
};