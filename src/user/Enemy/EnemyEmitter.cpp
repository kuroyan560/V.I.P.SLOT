#include "EnemyEmitter.h"
#include"KuroFunc.h"
#include"ObjectManager.h"
#include"ConstParameters.h"
#include"TimeScale.h"

void EnemyEmitter::EmitEnemy(std::weak_ptr<ObjectManager>& arg_objMgr, std::weak_ptr<CollisionManager>& arg_colMgr, int arg_type)
{
	if (arg_type == SLIDE_MOVE)
	{
		float y = KuroFunc::GetRand(ConstParameter::GameObject::POS_Y_MIN, ConstParameter::GameObject::POS_Y_MAX);

		arg_objMgr.lock()->AppearSlideMoveEnemy(
			arg_colMgr,
			0.2f,
			y);
	}
	else if (arg_type == SLIME_BATTERY)
	{
		float appearY = KuroFunc::GetRand(ConstParameter::GameObject::POS_Y_MIN, ConstParameter::GameObject::POS_Y_MAX);
		std::vector<float>destArray(KuroFunc::GetRand(3, 10));
		for (auto& destX : destArray)
		{
			destX = KuroFunc::GetRand(-ConstParameter::Environment::FIELD_WIDTH / 2.0f, ConstParameter::Environment::FIELD_WIDTH / 2.0f);
		}

		arg_objMgr.lock()->AppearSlimeBattery(
			arg_colMgr,
			appearY,
			destArray.data(),
			destArray.size());
	}
}

void EnemyEmitter::EmitEnemys(std::weak_ptr<ObjectManager>& arg_objMgr, std::weak_ptr<CollisionManager>& arg_colMgr)
{
	for (int num = 0; num < m_appearNum; ++num)
	{
		int par = KuroFunc::GetRand(99);
		int appearRate = 0;
		for (int typeIdx = 0; typeIdx < TYPE::NUM; ++typeIdx)
		{
			appearRate += m_appearRate[typeIdx];
			if (par < appearRate)
			{
				EmitEnemy(arg_objMgr,arg_colMgr, typeIdx);
				break;
			}
		}
	}
}

void EnemyEmitter::UpdateAppearRate(int arg_fixRateTypeIdx)
{
	int sum = 0;
	for (auto& rate : m_appearRate)sum += rate;
	while (100 < sum)
	{
		for (int typeIdx = 0; typeIdx < TYPE::NUM; ++typeIdx)
		{
			if (typeIdx == arg_fixRateTypeIdx)continue;

			m_appearRate[typeIdx]--;
		}
		sum = 0;
		for (auto& rate : m_appearRate)sum += rate;
	}
}

void EnemyEmitter::Init(std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<CollisionManager> arg_colMgr)
{
	EmitEnemys(arg_objMgr, arg_colMgr);
	m_timer.Reset(m_appearSpan);
}

void EnemyEmitter::TestRandEmit(const TimeScale& arg_timeScale, std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<CollisionManager> arg_colMgr)
{
	if (m_timer.UpdateTimer(arg_timeScale.GetTimeScale()))
	{
		EmitEnemys(arg_objMgr, arg_colMgr);
		m_timer.Reset(m_appearSpan);
	}
}

#include"imguiApp.h"
#include<magic_enum.h>
void EnemyEmitter::ImguiDebug()
{
	ImGui::Begin("EnemyEmitter");

	ImGui::Text("Timer : %.f", m_timer.GetLeftTime());

	ImGui::Separator();

	ImGui::DragInt("AppearNum", &m_appearNum, 1, 0);
	ImGui::DragInt("AppearSpan", &m_appearSpan, 1, 0);

	ImGui::Text("AppearRate");

	ImGui::BeginChild(ImGui::GetID((void*)0),ImVec2(250,100),ImGuiWindowFlags_NoTitleBar);
	for (int typeIdx = 0; typeIdx < TYPE::NUM; ++typeIdx)
	{
		auto type = (TYPE)typeIdx;
		if (ImGui::InputInt(std::string(magic_enum::enum_name(type)).c_str(), &m_appearRate[typeIdx]))
		{
			UpdateAppearRate(typeIdx);
		}
	}
	ImGui::EndChild();

	ImGui::End();
}
