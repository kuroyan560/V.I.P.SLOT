#include "EnemyEmitter.h"
#include"KuroFunc.h"
#include"ObjectManager.h"
#include"ConstParameters.h"
#include"TimeScale.h"
#include"GameObject.h"

void EnemyEmitter::RandEmitEnemy(TYPE arg_type)
{
	Vec3<float>initPos;
	initPos.z = ConstParameter::Environment::FIELD_FLOOR_POS.z;
	
	if (arg_type == TYPE::SLIDE_ENEMY_SLOW || arg_type == TYPE::SLIDE_ENEMY_FAST)
	{
		initPos.y = KuroFunc::GetRand(ConstParameter::GameObject::POS_Y_MIN, ConstParameter::GameObject::POS_Y_MAX);
	}
	else if (arg_type == TYPE::SLIME_BATTERY_ENEMY_NOMOVE)
	{
		initPos.x = KuroFunc::GetRand(-ConstParameter::Environment::FIELD_WIDTH / 2.0f, ConstParameter::Environment::FIELD_WIDTH / 2.0f);
		initPos.y = KuroFunc::GetRand(ConstParameter::GameObject::POS_Y_MIN, ConstParameter::GameObject::POS_Y_MAX);
	}
	else if (arg_type == TYPE::SLIME_BATTERY_ENEMY_CHASE_PLAYER)
	{
		initPos.x = ConstParameter::GameObject::POS_X_ABS * static_cast<float>(KuroFunc::GetRandPlusMinus());
		initPos.y = KuroFunc::GetRand(ConstParameter::GameObject::POS_Y_MIN, ConstParameter::GameObject::POS_Y_MAX);
	}
	EmitEnemy(arg_type, initPos);
}

void EnemyEmitter::RandEmitEnemys()
{
	for (int num = 0; num < m_appearNum; ++num)
	{
		int par = KuroFunc::GetRand(99);
		int appearRate = 0;
		for (int typeIdx = 0; typeIdx < static_cast<int>(TYPE::NUM); ++typeIdx)
		{
			appearRate += m_appearRate[typeIdx];
			if (par < appearRate)
			{
				RandEmitEnemy((TYPE)typeIdx);
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
		for (int typeIdx = 0; typeIdx < static_cast<int>(TYPE::NUM); ++typeIdx)
		{
			if (typeIdx == arg_fixRateTypeIdx)continue;

			m_appearRate[typeIdx]--;
		}
		sum = 0;
		for (auto& rate : m_appearRate)sum += rate;
	}
}

#include<magic_enum.h>
void EnemyEmitter::OnImguiItems()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::MenuItem("RandomAppear"))m_imguiDebugMode = IMGUI_RANDOM_APPEAR;
		if (ImGui::MenuItem("CustomParameter"))m_imguiDebugMode = IMGUI_CUSTOM_PARAM;
		ImGui::EndMenuBar();
	}

	if (m_imguiDebugMode == IMGUI_RANDOM_APPEAR)
	{
		//ランダム出現時間表示
		ImGui::Text("Timer : %.f", m_timer.GetLeftTime());

		ImGui::Separator();

		//ランダム出現の数と頻度
		ImGui::DragInt("AppearNum", &m_appearNum, 1, 0);
		ImGui::DragInt("AppearSpan", &m_appearSpan, 1, 0);

		//ランダム出現の種別ごとの出現確率
		ImGui::Text("AppearRate");
		for (int typeIdx = 0; typeIdx < static_cast<int>(TYPE::NUM); ++typeIdx)
		{
			auto type = (TYPE)typeIdx;
			if (ImGui::InputInt(std::string(magic_enum::enum_name(type)).c_str(), &m_appearRate[typeIdx],
				1,100,ImGuiInputTextFlags_CharsNoBlank))
			{
				UpdateAppearRate(typeIdx);
			}
		}
	}
	else if (m_imguiDebugMode == IMGUI_CUSTOM_PARAM)
	{
		ImGui::Text("CustomParameters");

		for (int typeIdx = 0; typeIdx < static_cast<int>(TYPE::NUM); ++typeIdx)
		{
			auto type = (TYPE)typeIdx;
			auto typeName = std::string(magic_enum::enum_name(type));

			if (ImGui::TreeNode(typeName.c_str()))
			{
				for (auto& param : m_customParams[type])
					ImGui::DragFloat(param.first.c_str(), &param.second, 0.02f);

				ImGui::TreePop();
			}
		}
	}
}

EnemyEmitter::EnemyEmitter() : Debugger("EnemyEmitter", ImGuiWindowFlags_MenuBar)
{
	m_customParams[TYPE::SLIDE_ENEMY_SLOW]["speed"] = 0.2f;
	m_customParams[TYPE::SLIDE_ENEMY_FAST]["speed"] = 0.5f;
	m_customParams[TYPE::SLIME_BATTERY_ENEMY_NOMOVE]["count"] = 3;
	m_customParams[TYPE::SLIME_BATTERY_ENEMY_CHASE_PLAYER]["count"] = 3;

	int rate = 100 / static_cast<int>(TYPE::NUM);
	for (auto& appearRate : m_appearRate)appearRate = rate;
}

void EnemyEmitter::Init()
{
	m_timer.Reset(0);
}

void EnemyEmitter::EmitEnemy(TYPE arg_type, Vec3<float>arg_initPos)
{
	std::weak_ptr<GameObject>enemyObj;
	if (arg_type == TYPE::SLIDE_ENEMY_SLOW || arg_type == TYPE::SLIDE_ENEMY_FAST)
	{
		enemyObj = m_referObjMgr.lock()->AppearSlideMoveEnemy(
			m_referCollisionMgr,
			m_customParams[arg_type]["speed"],
			arg_initPos);
	}
	else if (arg_type == TYPE::SLIME_BATTERY_ENEMY_NOMOVE)
	{
		std::vector<float>destArray(static_cast<int>(m_customParams[arg_type]["count"]), arg_initPos.x);

		enemyObj = m_referObjMgr.lock()->AppearSlimeBatteryRouteDefined(
			m_referCollisionMgr,
			arg_initPos,
			destArray.data(),
			destArray.size());
	}
	else if (arg_type == TYPE::SLIME_BATTERY_ENEMY_CHASE_PLAYER)
	{
		enemyObj = m_referObjMgr.lock()->AppearSlimeBatteryChasePlayer(
			m_referCollisionMgr, 
			arg_initPos, 
			static_cast<int>(m_customParams[arg_type]["count"]));
	}
	m_enemyObjList.push_front(enemyObj);
}

void EnemyEmitter::TestRandEmit(const TimeScale& arg_timeScale)
{
	if (m_timer.UpdateTimer(arg_timeScale.GetTimeScale()))
	{
		RandEmitEnemys();
		m_timer.Reset(m_appearSpan);
	}
}

int EnemyEmitter::KillAllEnemys()
{
	int coinNum = 0;
	for (auto& obj : m_enemyObjList)
	{
		coinNum += obj.lock()->Kill();
	}
	return coinNum;
}
