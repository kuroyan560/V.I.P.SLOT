#include "PlayersCoinUI.h"
#include"D3D12App.h"
#include"Sprite.h"

void PlayersCoinUI::SpriteUpdate(int arg_num)
{
	//�����𒲂ׂāA�K�v�ȃX�v���C�g�����v�Z
	m_useSpriteNum = KuroFunc::GetDigit(arg_num);

	//�X�v���C�g��������Ȃ���ΕK�v���ǉ�
	int spriteIdx = static_cast<int>(m_spriteArray.size());
	while (static_cast<int>(m_spriteArray.size()) < m_useSpriteNum)
	{
		auto name = "Sprite - NormaNum - " + std::to_string(++spriteIdx);
		m_spriteArray.emplace_back(std::make_shared<Sprite>(nullptr, name.c_str()));
	}

	//�e�N�X�`���̃C���f�b�N�X���
	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		int num = KuroFunc::GetSpecifiedDigitNum(arg_num, m_useSpriteNum - i - 1, false);
		m_spriteArray[i]->SetTexture(m_numTexArray[num]);
	}

	//���W�v�Z
	float offsetY = m_numPosOffset.y / static_cast<float>(m_useSpriteNum - 1);
	if (m_useSpriteNum <= 1)offsetY = 0.0f;
	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		Vec2<float>pos = m_numPos;
		pos.x -= m_useSpriteNum * m_numPosOffset.x;	//�E����
		pos.x += m_numPosOffset.x * static_cast<float>(i);
		pos.y -= offsetY * static_cast<float>(m_useSpriteNum - i - 1);
		m_spriteArray[i]->m_transform.SetPos(pos);
		m_spriteArray[i]->SendTransformBuff();
	}

	m_pastCoinNum = arg_num;
}

void PlayersCoinUI::Execute(int arg_num)
{
	SpriteUpdate(arg_num);
	m_staging.m_timer.Reset(m_staging.m_interval);
}

PlayersCoinUI::PlayersCoinUI()
{
	//�����R�C�����`��Ɏg�������e�N�X�`���ǂݍ���
	int texNum = 11;
	m_numTexArray.resize(texNum);
	D3D12App::Instance()->GenerateTextureBuffer(m_numTexArray.data(), "resource/user/img/ui/norma/norma_num.png", 11, Vec2<int>(11, 1));

	//�����X�v���C�g���w�肵�����������O�ɗp��
	static const int PREPARE_DIGIT_NUM = 5;
	m_spriteArray.resize(PREPARE_DIGIT_NUM);
	for (int i = 0; i < static_cast<int>(m_spriteArray.size()); ++i)
	{
		auto name = "Sprite - NormaNum - " + std::to_string(i);
		m_spriteArray[i] = std::make_shared<Sprite>(nullptr, name.c_str());
		m_spriteArray[i]->m_mesh.SetAnchorPoint({ 0.5f,0.5f });
		m_spriteArray[i]->m_mesh.SendBuff();
	}
}

void PlayersCoinUI::Init(int arg_initNum)
{
	SpriteUpdate(arg_initNum);
	m_staging.m_timer.Reset(0);
}

void PlayersCoinUI::Update(int arg_monitorNum, const float& arg_timeScale)
{
	//�������قȂ�Εω����o
	if (arg_monitorNum != m_pastCoinNum)Execute(arg_monitorNum);

	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		float scaleOffset = KuroMath::Ease(Out, Elastic, m_staging.m_timer.GetTimeRate(), m_staging.m_numScaleOffsetMax, 0.0f);
		m_spriteArray[i]->m_transform.SetScale(m_numScale + scaleOffset);
	}
	m_staging.m_timer.UpdateTimer(arg_timeScale);
}

void PlayersCoinUI::Draw2D()
{
	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		m_spriteArray[i]->Draw();
	}
}

#include"ImguiApp.h"
void PlayersCoinUI::ImguiDebug()
{
	ImGui::Begin("PlayersCoinUI");
	bool change = false;

	if (ImGui::TreeNode("CoinNum"))
	{
		int num = m_pastCoinNum;
		if (ImGui::InputInt("Num", &num))Execute(num);
		if (ImGui::DragFloat2("Pos", (float*)&m_numPos))change = true;
		if (ImGui::DragFloat2("Offset", (float*)&m_numPosOffset))change = true;
		if (ImGui::DragFloat("Scale", (float*)&m_numScale, 0.05f))change = true;

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Staging"))
	{
		if (ImGui::Button("Preview"))Execute(m_pastCoinNum);
		ImGui::DragFloat("Interval", &m_staging.m_interval, 0.5f);
		ImGui::DragFloat("ScaleOffsetMax", &m_staging.m_numScaleOffsetMax, 0.05f);
		ImGui::TreePop();
	}

	ImGui::End();

	if (change)SpriteUpdate(m_pastCoinNum);
}
