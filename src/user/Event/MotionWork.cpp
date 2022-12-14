#include "MotionWork.h"

bool MotionWork::Update(float arg_timeScale)
{
	if (m_motionWorkIdx < 0)return false;
	if (static_cast<int>(m_motions.size()) <= m_motionWorkIdx)return false;

	bool end = m_timer.UpdateTimer(arg_timeScale);

	//現在進行中のモーションの参照取得
	const auto& nowWork = m_motions[m_motionWorkIdx];
	m_nowPos = nowWork.m_easeParam.Calculate(
		m_timer.GetTimeRate(),
		nowWork.m_startPos,
		nowWork.m_endPos);

	m_nowTarget = nowWork.m_easeParam.Calculate(
		m_timer.GetTimeRate(),
		nowWork.m_startTarget,
		nowWork.m_endTarget);

	//自動で次のモーションへ
	if (end && m_auto)
	{
		m_motionWorkIdx++;
		if (m_motionWorkIdx < static_cast<int>(m_motions.size()))
			m_timer.Reset(m_motions[m_motionWorkIdx].m_interval);
	}

	return end;
}

void MotionWork::Start(bool arg_auto)
{
	m_motionWorkIdx = 0;
	m_timer.Reset(m_motions[m_motionWorkIdx].m_interval);
	m_auto = arg_auto;
}

void MotionWork::Proceed()
{
	//自動モードなら何もしない
	if (m_auto)return;
	m_motionWorkIdx++;
	if (m_motionWorkIdx < static_cast<int>(m_motions.size()))
		m_timer.Reset(m_motions[m_motionWorkIdx].m_interval);
}

#include"imguiApp.h"
void MotionWork::ImguiDebugItems(std::string arg_tag, const Vec3<float>* arg_changePos, const Vec3<float>* arg_changeTarget)
{
	arg_changePos = nullptr;
	arg_changeTarget = nullptr;

	int eraseIdx = -1;
	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 220), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < static_cast<int>(m_motions.size()); ++i)
	{
		std::string numLabel = std::to_string(i);
		std::string fullLabel = arg_tag + " - " + numLabel;

		auto& motion = m_motions[i];

		if (ImGui::TreeNode(fullLabel.c_str()))
		{
			if (ImGui::DragFloat3("StartPos", (float*)&motion.m_startPos, 0.02f))arg_changePos = &motion.m_startPos;
			if (ImGui::DragFloat3("EndPos", (float*)&motion.m_endPos, 0.02f))arg_changePos = &motion.m_endPos;
			if (ImGui::DragFloat3("StartTarget", (float*)&motion.m_startTarget, 0.02f))arg_changeTarget = &motion.m_startTarget;
			if (ImGui::DragFloat3("EndTarget", (float*)&motion.m_endTarget, 0.02f))arg_changeTarget = &motion.m_endTarget;
			ImGui::DragFloat("Interval", &motion.m_interval, 0.02f);

			motion.m_easeParam.ImguiDebug(fullLabel.c_str());

			ImGui::TreePop();
		}
		ImGui::SameLine();
		if (ImGui::Button((numLabel + "-").c_str()))
		{
			eraseIdx = i;
		}
	}
	ImGui::EndChild();

	if (eraseIdx != -1)m_motions.erase(m_motions.begin() + eraseIdx);
	if (ImGui::Button("+"))m_motions.emplace_back();
}
