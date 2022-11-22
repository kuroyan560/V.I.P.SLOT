#include "TitleUI.h"
#include"D3D12Data.h"

void TitleUI::UpdateItemPosOffset()
{
	int itemNum = static_cast<int>(m_items.size());

	//一番上の項目のY座標
	float offsetYMin = static_cast<float>(-itemNum / 2);
	if (itemNum % 2 == 0)offsetYMin += 0.5f;
	offsetYMin *= m_itemOffsetY;

	for (int i = 0; i < itemNum; ++i)
	{
		m_items[i].m_posOffset.y = offsetYMin + m_itemOffsetY * i + m_items[i].m_tex.m_back->GetGraphSize().y / 2.0f;
	}
}

TitleUI::TitleUI()
{
	m_randBox.SetColor(Color(255, 93, 204, 255));
	m_randBox.m_interval = 27;
	m_randBox.m_maxVal = { 190.0f,195.0f };
}

void TitleUI::Awake(const std::vector<TitleItemTex>& arg_tex)
{
	//項目数
	int itemNum = static_cast<int>(arg_tex.size());
	for (int i = 0; i < itemNum; ++i)
	{
		m_items.emplace_back();
		m_items.back().m_tex = arg_tex[i];
	}

	UpdateItemPosOffset();
}

void TitleUI::Init()
{
	m_randBox.Init();
}

void TitleUI::Update()
{
	m_randBox.Update();
}

#include"DrawFunc2D.h"
void TitleUI::Draw(int arg_selectIdx)
{
	for (int itemIdx = 0; itemIdx < static_cast<int>(m_items.size()); ++itemIdx)
	{
		//選択中の項目ならスルー
		if (itemIdx == arg_selectIdx)continue;

		const auto& item = m_items[itemIdx];

		//バック画像
		DrawFunc2D::DrawGraph(m_itemBasePos + item.m_posOffset, item.m_tex.m_back);
		//フロント画像
		DrawFunc2D::DrawGraph(m_itemBasePos + item.m_posOffset, item.m_tex.m_front);
	}

	//選択中の項目は描画順最後
	const auto& item = m_items[arg_selectIdx];

	//バック画像
	DrawFunc2D::DrawGraph(m_itemBasePos + item.m_posOffset, item.m_tex.m_back);

	//選択強調の歪み四角
	auto backGraphSize = item.m_tex.m_back->GetGraphSize().Float();
	m_randBox.Transform().SetPos({ m_itemBasePos + item.m_posOffset + Vec2<float>(backGraphSize.x / 2.0f , backGraphSize.y / 2.0f) });
	m_randBox.SetSize(backGraphSize * 0.8f);
	m_randBox.SetAnchorPoint({ 0.5f,0.5f });
	m_randBox.Draw();

	//フロント画像
	DrawFunc2D::DrawGraph(m_itemBasePos + item.m_posOffset, item.m_tex.m_front);

}

#include"imguiApp.h"
void TitleUI::ImguiDebug()
{
	ImGui::Begin("TitleUI");

	ImGui::DragFloat("ItemBasePosX", &m_itemBasePos.x);
	ImGui::DragFloat("ItemBasePosY", &m_itemBasePos.y);

	if (ImGui::DragFloat("ItemOffsetY", &m_itemOffsetY))
	{
		UpdateItemPosOffset();
	}

	ImGui::End();

	m_randBox.ImguiDebug();
}
