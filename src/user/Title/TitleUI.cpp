#include "TitleUI.h"
#include"D3D12Data.h"

void TitleUI::UpdateItemPosOffset()
{
	int itemNum = static_cast<int>(m_items.size());

	//��ԏ�̍��ڂ�Y���W
	float offsetYMin = static_cast<float>(-itemNum / 2);
	if (itemNum % 2 == 0)offsetYMin += 0.5f;
	offsetYMin *= m_itemOffsetY;

	for (int i = 0; i < itemNum; ++i)
	{
		m_items[i].m_posOffset.y = offsetYMin + m_itemOffsetY * i + m_items[i].m_tex.m_back->GetGraphSize().y / 2.0f;
	}
}

void TitleUI::OnImguiItems()
{
	ImGui::DragFloat("ItemBasePosX", &m_itemBasePos.x);
	ImGui::DragFloat("ItemBasePosY", &m_itemBasePos.y);

	if (ImGui::DragFloat("ItemOffsetY", &m_itemOffsetY))
	{
		UpdateItemPosOffset();
	}
}

TitleUI::TitleUI() : Debugger("TitleUI")
{
	m_randBox.SetColor(Color(255, 93, 204, 255));
	m_randBox.m_interval = 27;
	m_randBox.m_maxVal = { 190.0f,195.0f };
}

void TitleUI::Awake(const std::vector<TitleItemTex>& arg_tex)
{
	//���ڐ�
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
	m_appear = false;
	m_randBox.Init();
}

void TitleUI::Update()
{
	//�o�ꂵ�Ă��Ȃ�
	if (!m_appear)return;

	if (m_appearWaitTimer.UpdateTimer(1.0f))
	{
		m_appearTimer.UpdateTimer(1.0f);
	}

	m_randBox.Update();
}

#include"DrawFunc2D.h"
void TitleUI::Draw(int arg_selectIdx, bool arg_isSelect)
{
	//�o�ꂵ�Ă��Ȃ�
	if (!m_appear)return;

	Vec2<float>appearOffset =
	{
		KuroMath::Ease(Out,Elastic,m_appearTimer.GetTimeRate(),-300.0f,0.0f),
		0.0f
	};
	appearOffset = { 0,0 };

	Vec2<float>scale =
	{
		KuroMath::Ease(Out, Elastic, m_appearTimer.GetTimeRate(), 0.0f, 1.0f),
		KuroMath::Ease(Out, Elastic, m_appearTimer.GetTimeRate(), 0.0f, 1.0f),
	};

	for (int itemIdx = 0; itemIdx < static_cast<int>(m_items.size()); ++itemIdx)
	{
		//�I�𒆂̍��ڂȂ�X���[
		if (arg_isSelect && itemIdx == arg_selectIdx)continue;

		const auto& item = m_items[itemIdx];

		//�o�b�N�摜
		DrawFunc2D::DrawRotaGraph2D(m_itemBasePos + item.m_posOffset + appearOffset, scale, 0.0f, item.m_tex.m_back);
		//�t�����g�摜
		DrawFunc2D::DrawRotaGraph2D(m_itemBasePos + item.m_posOffset + appearOffset, scale, 0.0f, item.m_tex.m_front);
	}

	//�I�𒆂̍��ڂ͕`�揇�Ō�
	//�o�b�N�摜�ƃt�����g�摜�̊Ԃɘc�ݎl�p�`�������
	if (arg_isSelect)
	{
		const auto& item = m_items[arg_selectIdx];

		//�o�b�N�摜
		DrawFunc2D::DrawRotaGraph2D(m_itemBasePos + item.m_posOffset + appearOffset, scale, 0.0f, item.m_tex.m_back);

		//�I�������̘c�ݎl�p
		auto backGraphSize = item.m_tex.m_back->GetGraphSize().Float();
		m_randBox.Transform().SetPos(m_itemBasePos + item.m_posOffset + appearOffset);
		m_randBox.SetSize(backGraphSize * 0.8f);
		m_randBox.SetAnchorPoint({ 0.5f,0.5f });
		m_randBox.Draw();

		//�t�����g�摜
		DrawFunc2D::DrawRotaGraph2D(m_itemBasePos + item.m_posOffset + appearOffset, scale, 0.0f, item.m_tex.m_front);
	}
}