#include "PrecisionAimAbility.h"
#include"DrawFunc2D.h"

void PrecisionAimAbility::Draw()
{
	//”­“®’†‚©
	if (!Ability::GetInvokeNotify())return;

	DrawFunc2D::DrawBox2D({ 50,50 }, { 200,200 }, Color(), true);
}
