#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"Importer.h"
#include"GameCamera.h"
#include"TimeScale.h"
#include"Player.h"

bool SlotMachine::CheckReelPattern()
{
	const auto& pattern = m_reels[REEL::LEFT].GetNowPattern();

	//�G�����Ⴆ�Ή��������I��
	if (pattern != m_reels[REEL::CENTER].GetNowPattern())return false;
	if (pattern != m_reels[REEL::RIGHT].GetNowPattern())return false;

	m_patternMgr.Invoke(pattern);
	return true;
}

void SlotMachine::SlotPerform(const ConstParameter::Slot::PATTERN& arg_pattern)
{
	using namespace ConstParameter::Slot;
	using PATTERN = ConstParameter::Slot::PATTERN;
}

SlotMachine::SlotMachine()
{
	//�X���b�g�}�V������
	m_slotMachineObj = std::make_shared<ModelObject>("resource/user/model/", "slotMachine.glb");

	//���f�����烊�[���̏��擾
	for (auto& mesh : m_slotMachineObj->m_model->m_meshes)
	{
		//�}�e���A�����擾
		const auto& materialName = mesh.material->name;

		for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)
		{
			//���[���̃��b�V������
			if (REEL_MATERIAL_NAME[reelIdx].compare(materialName) == 0)
			{
				//���[���̃��b�V���|�C���^���A�^�b�`
				m_reels[reelIdx].Attach(&mesh);
			}
		}
	}

	//�R�C�����������K�z������
	m_megaPhoneObj = std::make_shared<ModelObject>("resource/user/model/", "megaPhone.glb");
	//�X���b�g�}�V���̃g�����X�t�H�[����e�Ƃ��ēo�^
	m_megaPhoneObj->m_transform.SetParent(&m_slotMachineObj->m_transform);
	//���K�z���ʒu
	m_megaPhoneObj->m_transform.SetPos(ConstParameter::Slot::MEGA_PHONE_POS);
	//�g�����X�t�H�[���o�b�t�@�X�V�̂��ߌĂяo��
	m_megaPhoneObj->GetTransformBuff();

	//�T�E���h�ǂݍ���
	m_spinStartSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_start.wav");
	m_reelStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_stop.wav");
}

void SlotMachine::Init()
{
	//�f�o�b�O�p
	using PATTERN = ConstParameter::Slot::PATTERN;
	std::vector<PATTERN>testPatternArray;
	testPatternArray.resize(20);
	for (int i = 0; i < 20; ++i)
	{
		testPatternArray[i] = PATTERN::NONE;
	}

	//���[��������
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init(nullptr, testPatternArray);

	//���o�[������
	m_lever = -1;

	//�Ō�̃��[�����~�߂Ă���̎��Ԍv���p�^�C�}�[���Z�b�g
	m_slotWaitTimer.Reset(ConstParameter::Slot::SLOT_WAIT_TIME);
}

//�f�o�b�O�p
#include"UsersInput.h"

void SlotMachine::Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale)
{
	//���[���X�V
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Update(arg_timeScale.GetTimeScale());

	//�S���[����~��
	if (REEL::RIGHT < m_lever)
	{
		//���̃X���b�g��]�\��
		if (m_slotWaitTimer.UpdateTimer(arg_timeScale.GetTimeScale()))
		{
			m_lever = -1;
			m_slotWaitTimer.Reset();
		}
	}
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_megaPhoneObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
}

void SlotMachine::Lever()
{
	//�X���b�g��]�J�n
	if (m_lever == -1)
	{
		for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Start();
		AudioApp::Instance()->PlayWaveDelay(m_spinStartSE);
		m_lever++;
	}
	//�e���[����~
	else if (m_lever <= REEL::RIGHT && m_reels[m_lever].IsCanStop())
	{
		m_reels[m_lever].Stop();
		AudioApp::Instance()->PlayWaveDelay(m_reelStopSE);

		//�G�����m�F���đS�Ĉꏏ�Ȃ���ʔ���
		if (m_lever++ == REEL::RIGHT && CheckReelPattern())
		{
			//���ʂ��m�F���ĉ��o�̏���
			SlotPerform(m_reels[REEL::LEFT].GetNowPattern());
		}
	}
}