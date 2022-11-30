#include "WaveMgr.h"
#include"Sprite.h"
#include"D3D12App.h"
#include"KuroFunc.h"

void WaveMgr::OnImguiItems()
{
	bool change = false;

	auto strPos = m_normaStrSprite->m_transform.GetPos();
	if (ImGui::DragFloat2("StrPos", (float*)&strPos))
	{
		m_normaStrSprite->m_transform.SetPos(strPos);
		m_normaStrSprite->SendTransformBuff();
	}
	auto strScale = m_normaStrSprite->m_transform.GetScale().x;
	if (ImGui::DragFloat("StrScale", &strScale, 0.05f))
	{
		m_normaStrSprite->m_transform.SetScale(strScale);
		m_normaStrSprite->SendTransformBuff();
	}

	if (ImGui::DragFloat2("NumPos", (float*)&m_numPos))change = true;
	if (ImGui::DragFloat2("NumOffset", (float*)&m_numPosOffset))change = true;
	if (ImGui::DragFloat("NumScale", &m_numScale, 0.05f))change = true;

	ImGui::Separator();

	ImGui::Text("Norma : { %d }", m_norma);

	ImGui::Separator();

	ImGui::Checkbox("InfinityMode", &m_isInfinity);

	int norma = m_norma;
	if (ImGui::InputInt("Norma", &norma))change = true;

	if (change)
	{
		Init(norma);
	}
}

WaveMgr::WaveMgr() : Debugger("WaveMgr")
{
	Vec2<float>NORMA_STR_POS = { 1080.0f,74.0f };
	float NORMA_STR_SCALE = 0.9f;

	auto app = D3D12App::Instance();

	/*--- �m���}�\�� ---*/
	std::string normaTexDir = "resource/user/img/ui/norma/";
	m_normaStrSprite = std::make_shared<Sprite>(app->GenerateTextureBuffer(normaTexDir + "norma_str.png"), "Sprite - NormaStr");
	m_normaStrSprite->m_transform.SetPos(NORMA_STR_POS);
	m_normaStrSprite->m_transform.SetScale(NORMA_STR_SCALE);
	m_normaStrSprite->SendTransformBuff();

	//�����X�v���C�g���w�肵�����������O�ɗp��
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

	//�����𒲂ׂāA�K�v�ȃX�v���C�g�����v�Z
	int normaDigit = KuroFunc::GetDigit(m_norma);
	//�u / �v���ǉ�
	m_useSpriteNum = normaDigit + 1;

	//�X�v���C�g��������Ȃ���ΕK�v���ǉ�
	int spriteIdx = static_cast<int>(m_normaNumSpriteArray.size());
	while (static_cast<int>(m_normaNumSpriteArray.size()) < m_useSpriteNum)
	{
		auto name = "Sprite - NormaNum - " + std::to_string(++spriteIdx);
		m_normaNumSpriteArray.emplace_back(std::make_shared<Sprite>(nullptr, name.c_str()));
	}

	//�ŏ��̃e�N�X�`���́u / �v
	m_normaNumSpriteArray[0]->SetTexture(m_normaTexArray[10]);
	//�e�N�X�`���̃C���f�b�N�X���
	for (int i = 1; i < m_useSpriteNum; ++i)
	{
		int num = KuroFunc::GetSpecifiedDigitNum(m_norma, m_useSpriteNum - i - 1, false);
		m_normaNumSpriteArray[i]->SetTexture(m_normaTexArray[num]);
	}

	//���W�v�Z
	float offsetY = m_numPosOffset.y / static_cast<float>(m_useSpriteNum - 1);
	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		Vec2<float>pos = m_numPos;
		pos.x -= m_useSpriteNum * m_numPosOffset.x;	//�E����
		pos.x += m_numPosOffset.x * static_cast<float>(i);
		pos.y -= offsetY * static_cast<float>(m_useSpriteNum - i - 1);
		m_normaNumSpriteArray[i]->m_transform.SetPos(pos);
		m_normaNumSpriteArray[i]->m_transform.SetScale(m_numScale);
		m_normaNumSpriteArray[i]->SendTransformBuff();
	}
}

void WaveMgr::OnDraw2D()
{
	//�m���}�����`��
	for (int i = 0; i < m_useSpriteNum; ++i)
	{
		m_normaNumSpriteArray[i]->Draw();
	}

	//�m���}����
	m_normaStrSprite->Draw();
}