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
	//�g�����X�t�H�[������
	m_slotMachineObj->m_transform.SetScale(0.4f);

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

	//���[�������G���Z�b�g�i�f�o�b�O�p�A�������ʂȂ��j
		//�f�o�b�O�p
	using PATTERN = ConstParameter::Slot::PATTERN;
	std::vector<PATTERN>testPatternArray;
	testPatternArray.resize(20);
	for (int i = 0; i < 20; ++i)
	{
		testPatternArray[i] = PATTERN::NONE;
	}

	//���[��������
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].SetPattern(nullptr, testPatternArray);
}

void SlotMachine::Init()
{
	//m_slotMachineObj->m_transform.SetScale(1.4f);
	m_slotMachineObj->m_transform.SetScale(2.0f);
	m_slotMachineObj->m_transform.SetPos({ 0.0f,3.0f,0.0f });

	//���o�[������
	m_lever = REEL::NONE;

	//�X���b�g��]�\�񃊃Z�b�g
	m_startSlotCount = 0;

	//���[��������
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init();
}

void SlotMachine::Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Slot;

	const float timeScale = arg_timeScale.GetTimeScale();

	//�������쎞�Ԍv���̃^�C���X�P�[��
	//�Q�[�W�ʂ������قǎ��Ԃ����܂�
	const float autoOperateTimeScale =
		KuroMath::Lerp(1.0f,
			AUTO_OPERATE_TIME_SCALE_MAX,
			KuroMath::GetRateInRange(
				DEFAULT_TIME_SCALE_SLOT_GAUGE_NUM,
				MAX_TIME_SCALE_SLOT_GAUGE_NUM,
				m_startSlotCount)) * timeScale;

	//���[���X�V
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Update(timeScale);

	//��~��
	if (m_lever == REEL::NONE)
	{
		//�X���b�g�\�񂪂���
		if (m_startSlotCount)
		{
			//�\�񐔏���
			m_startSlotCount--;
			//�X���b�g�X�^�[�g
			Lever();
			//�ŏ��̃��[����~�܂ł̃^�C�}�[�Z�b�g
			m_autoTimer[AUTO_OPERATE::UNTIL_FIRST_REEL].Reset(AUTO_OPERATE_TIME[AUTO_OPERATE::UNTIL_FIRST_REEL]);
		}
	}
	//�ғ���
	else if(m_lever < REEL::NUM)
	{
		m_autoTimer[AUTO_OPERATE::UNTIL_FIRST_REEL].UpdateTimer(autoOperateTimeScale);
		m_autoTimer[AUTO_OPERATE::REEL_STOP_SPAN].UpdateTimer(autoOperateTimeScale);

		//���[����~
		if (m_autoTimer[AUTO_OPERATE::UNTIL_FIRST_REEL].IsTimeUpOnTrigger()
			|| m_autoTimer[AUTO_OPERATE::REEL_STOP_SPAN].IsTimeUpOnTrigger())
		{
			if (Lever())
			{
				//�Ō�̃��[����~
				m_autoTimer[AUTO_OPERATE::AFTER_STOP_ALL_REEL].Reset(
					AUTO_OPERATE_TIME[AUTO_OPERATE::AFTER_STOP_ALL_REEL]);
			}
			else
			{
				//���̃��[����
				m_autoTimer[AUTO_OPERATE::REEL_STOP_SPAN].Reset(
					AUTO_OPERATE_TIME[AUTO_OPERATE::REEL_STOP_SPAN]);
			}
		}

	}
	else
	{
		if (m_autoTimer[AUTO_OPERATE::AFTER_STOP_ALL_REEL].UpdateTimer(autoOperateTimeScale))
		{
			m_lever = REEL::NONE;
		}
	}
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
	//DrawFunc3D::DrawNonShadingModel(m_megaPhoneObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
}

#include"imguiApp.h"
void SlotMachine::ImguiDebug()
{
	ImGui::Begin("SlotMachine");
	ImGui::Text("StartSlotCount : { %d }", m_startSlotCount);

	using namespace ConstParameter::Slot;
	const float autoOperateTimeScale =
		KuroMath::Lerp(1.0f,
			AUTO_OPERATE_TIME_SCALE_MAX,
			KuroMath::GetRateInRange(
				DEFAULT_TIME_SCALE_SLOT_GAUGE_NUM,
				MAX_TIME_SCALE_SLOT_GAUGE_NUM,
				m_startSlotCount));
	ImGui::Text("AutoOperateTimeScale : { %f }", autoOperateTimeScale);

	ImGui::End();
}

bool SlotMachine::Lever()
{
	//�X���b�g��]�J�n
	if (m_lever == REEL::NONE)
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
			return true;
		}
	}
	return false;
}

void SlotMachine::Booking()
{
	//�X���b�g�\��i�X���b�g�Q�[�W�j�A����𒴂��Ȃ��悤��
	m_startSlotCount = std::min(ConstParameter::Slot::SLOT_GAUGE_MAX, m_startSlotCount + 1);
}

void SlotMachine::ReelSet(REEL arg_which,
	std::shared_ptr<TextureBuffer>& arg_reelTex,
	std::vector<ConstParameter::Slot::PATTERN>& arg_patternArray)
{
	m_reels[arg_which].SetPattern(arg_reelTex, arg_patternArray);
}
