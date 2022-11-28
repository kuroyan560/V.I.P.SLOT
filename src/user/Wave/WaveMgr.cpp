#include "WaveMgr.h"
#include"Sprite.h"
#include"D3D12App.h"

WaveMgr::WaveMgr()
{
	auto app = D3D12App::Instance();

	/*--- ノルマ表示 ---*/
	std::string normaTexDir = "resource/user/img/ui/norma/";
	m_normaStrSprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(normaTexDir + "norma_str.png"), "Sprite - NormaStr");
	//数字スプライトを指定した桁数分事前に用意
	static const int PREPARE_DIGIT_NUM = 5;
	m_normaNumSpriteArray.resize(PREPARE_DIGIT_NUM);
	for (int i = 0; i < static_cast<int>(m_normaNumSpriteArray.size()); ++i)
	{
		auto name = "Sprite - NormaNum - " + std::to_string(i);
		m_normaNumSpriteArray[i] = std::make_shared<Sprite>(nullptr, name.c_str());
	}

	int texNum = 11;
	m_normaTexArray.resize(texNum);
	app->GenerateTextureBuffer(m_normaTexArray.data(), normaTexDir + "norma_num.png", texNum, Vec2<int>(11, 1));
}

void WaveMgr::Init(int arg_norma)
{
	m_norma = arg_norma;
}

#include"ImguiApp.h"
void WaveMgr::ImguiDebug()
{
	ImGui::Begin("WaveMgr");

	ImGui::Text("Norma : { %d }", m_norma);

	ImGui::Separator();

	ImGui::Checkbox("InfinityMode", &m_isInfinity);

	int norma = m_norma;
	if (ImGui::InputInt("Norma", &norma))
	{
		Init(norma);
	}

	ImGui::End();
}
