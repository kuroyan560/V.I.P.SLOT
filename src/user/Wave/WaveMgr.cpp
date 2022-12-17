#include "WaveMgr.h"
#include"Sprite.h"
#include"D3D12App.h"
#include"KuroFunc.h"
#include"TimeScale.h"
#include"EnemyEmitter.h"

void WaveMgr::OnImguiItems()
{
	ImGui::Checkbox("InfinityMode", &m_isInfinity);
	ImGui::Text("NowWaveIdx : { %d / %d }", m_nowWaveIdx, static_cast<int>(m_waves.size()));
	ImGui::Text("Norma : { %d }", m_sumNorma);
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

void WaveMgr::Init(std::list<Wave>arg_waves)
{
	//�E�F�[�u�z��i�[
	m_waves = arg_waves;
	//�E�F�[�u�C���f�b�N�X������
	m_nowWaveIdx = 0;
	//�S�E�F�[�u�N���A�t���O������
	m_isAllWaveClear = false;
	//�ʎZ�m���}������
	m_sumNorma = 0;

	//�E�F�[�u�i�s���̏������Ăяo��
	WaveInit();
}

void WaveMgr::Update(const TimeScale& arg_timeScale, std::weak_ptr<EnemyEmitter>arg_enemyEmitter)
{
	//��񂪖�����΃e�X�g�p�̃����_���o��
	if (m_nowInfoArray.empty())
	{
		arg_enemyEmitter.lock()->TestRandEmit(arg_timeScale);
		return;
	}

	m_time += arg_timeScale.GetTimeScale();

	for (auto itr = m_nowInfoArray.begin(); itr != m_nowInfoArray.end();)
	{
		//�o���^�C�~���O�łȂ�
		if (m_time < itr->m_info->m_appearTiming)continue;

		//���[�v�o���łȂ�
		if (!itr->m_info->m_loopAppear)
		{
			//�o��
			arg_enemyEmitter.lock()->EmitEnemy(itr->m_info->m_type, itr->m_info->m_initPos);
			//�o���ςȂ̂ŏo������z�񂩂�폜
			itr = m_nowInfoArray.erase(itr);
			continue;
		}

		//���[�v�o��
		if (itr->m_timer.UpdateTimer(arg_timeScale.GetTimeScale()))
		{
			//�o��
			arg_enemyEmitter.lock()->EmitEnemy(itr->m_info->m_type, itr->m_info->m_initPos);
			//�o���^�C�}�[���Z�b�g
			itr->m_timer.Reset(itr->m_info->m_appearTiming);
		}
	}
}

void WaveMgr::ProceedWave()
{
	//�S�E�F�[�u�N���A������
	if (static_cast<int>(m_waves.size()) <= m_nowWaveIdx + 1)
	{
		m_isAllWaveClear = true;
		return;
	}

	//�E�F�[�u�i�s
	m_nowWaveIdx++;

	//�E�F�[�u������
	WaveInit();
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

void WaveMgr::WaveInit()
{
	//�E�F�[�u���ԏ�����
	m_time = 0.0f;

	//�E�F�[�u�̃|�C���^�擾
	int offset = 0;
	auto itr = m_waves.begin();
	while (offset < m_nowWaveIdx)
	{
		itr++;
		offset++;
	}
	m_nowWave = &(*itr);

	//�ʎZ�m���}�X�V
	m_sumNorma += m_nowWave->m_norma;

	/*--- �G�̏o�����z��p�� ---*/
	m_nowInfoArray.clear();
	for (const auto& info : m_nowWave->m_appearInfoList)
	{
		m_nowInfoArray.emplace_back();
		m_nowInfoArray.back().m_info = &info;
		m_nowInfoArray.back().m_timer.Reset(0.0f);
	}

	/*--- UI�X�V ---*/

	//�����𒲂ׂāA�K�v�ȃX�v���C�g�����v�Z
	int normaDigit = KuroFunc::GetDigit(m_sumNorma);
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
		int num = KuroFunc::GetSpecifiedDigitNum(m_sumNorma, m_useSpriteNum - i - 1, false);
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
