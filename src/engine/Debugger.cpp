#include "Debugger.h"
#include"Fps.h"

std::vector<Debugger*>Debugger::s_debuggerArray;
int Debugger::s_id = 0;

void Debugger::Draw()
{
	ImGui::Begin("DebuggerMgr");
	Fps::Instance()->OnImguiItems();
	ImGui::Separator();
	for (auto& debugger : s_debuggerArray)
	{
		ImGui::Checkbox(debugger->m_title.c_str(), &debugger->m_active);
	}
	ImGui::End();

	for (auto& debugger : s_debuggerArray)
	{
		if (!debugger->m_active)continue;
		ImGui::Begin(debugger->m_title.c_str(), nullptr, debugger->m_imguiWinFlags);
		debugger->OnImguiItems();
		ImGui::End();
	}
}
