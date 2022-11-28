#include "WaveMgr.h"
#include"Sprite.h"
#include"D3D12App.h"
#include"KuroFunc.h"

WaveMgr::WaveMgr()
{
	Vec2<float>NORMA_STR_POS = { 1092.0f,76.0f };

	auto app = D3D12App::Instance();

	/*--- ノルマ表示 ---*/
	std::string normaTexDir = "resource/user/img/ui/norma/";
	m_normaStrSprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(normaTexDir + "norma_str.png"), "Sprite - NormaStr");
	m_normaStrSprite->m_transform.SetPos(NORMA_STR_POS);
	m_normaStrSprite->SendTransformBuff();

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
	if (m_norma <= 0)m_norma = 1;

	//桁数を調べて、必要なスプライト数を計算
	int normaDigit = KuroFunc::GetDigit(m_norma);
	//「 / 」分追加
	m_useSpriteNum = normaDigit + 1;

	//スプライト数が足りなければ必要数追加
	int spriteIdx = static_cast<int>(m_normaNumSpriteArray.size());
	while (static_cast<int>(m_normaNumSpriteArray.size()) < m_useSpriteNum)
	{
		auto name = "Sprite - NormaNum - " + std::to_string(++spriteIdx);
		m_normaNumSpriteArray.emplace_back(std::make_shared<Sprite>(nullptr, name.c_str()));
	}

	//最初のテクスチャは「 / 」
	m_normaNumSpriteArray[0]->SetTexture(m_normaTexArray[10]);
	//テクスチャのインデックス情報
	for (int i = 1; i < m_useSpriteNum; ++i)
	{
		int num = KuroFunc::GetSpecifiedDigitNum(m_norma, m_useSpriteNum - i - 1, false);
		m_normaNumSpriteArray[i]->SetTexture(m_normaTexArray[num]);
	}

}

void WaveMgr::Draw2D()
{
	//ノルマ数
	float offsetY = m_numPosOffset.y / static_cast<float>(m_useSpriteNum - 1);
	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		Vec2<float>pos = m_numPos;
		pos.x -= m_useSpriteNum * m_numPosOffset.x;	//右揃え
		pos.x += m_numPosOffset.x * static_cast<float>(i);
		pos.y += offsetY * static_cast<float>(i);
		m_normaNumSpriteArray[i]->m_transform.SetPos(pos);
		m_normaNumSpriteArray[i]->Draw();
	}
	//ノルマ文字
	m_normaStrSprite->Draw();
}

#include"ImguiApp.h"
void WaveMgr::ImguiDebug()
{
	ImGui::Begin("WaveMgr");

	ImGui::DragFloat2("NumPos", (float*)&m_numPos);
	ImGui::DragFloat2("NumOffset", (float*)&m_numPosOffset);

	ImGui::Separator();

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
