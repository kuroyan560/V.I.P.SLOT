#include"HealAbility.h"
#include"DrawFunc2D.h"

void HealAbility::Draw()
{
	//”­“®’†‚©
	if (!Ability::GetInvokeNotify())return;

	DrawFunc2D::DrawBox2D({ 50,50 }, { 200,200 }, Color(0.0f, 1.0f, 0.0f, 1.0f), true);
}