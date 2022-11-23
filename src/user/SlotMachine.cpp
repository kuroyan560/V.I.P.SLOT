#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"Importer.h"
#include"GameCamera.h"
#include"TimeScale.h"
#include"Player.h"
#include"KuroEngine.h"
#include"DrawFunc2D.h"

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

void SlotMachine::UpdateSlotGaugeScreen()
{
	using namespace ConstParameter::Slot;

	auto& cmdList = D3D12App::Instance()->GetCmdList();
	m_slotGaugeScreen->Clear(cmdList);

	KuroEngine::Instance()->Graphics().SetRenderTargets({ m_slotGaugeScreen });

	//�X���b�g�Q�[�W�t����ʉ摜�T�C�Y
	const auto slotGaugeScreenSize = WinApp::Instance()->GetWinSize().Float();;
	//�X���b�g�Q�[�W�摜�T�C�Y
	const auto slotGaugeTexSize = m_slotGaugeTex->GetGraphSize().Float();

	//�X���b�g�Q�[�W��`�悷��̈�̃��[�g
	const Vec2<float>slotGaugeDrawScreenSizeRate = { 0.95f,0.7f };
	//const Vec2<float>slotGaugeDrawScreenSizeRate = { 1.0f,1.0f };
	//�X���b�g�Q�[�W��`�悷��̈�
	const Vec2<float>slotGaugeDrawScreenSize = slotGaugeScreenSize * slotGaugeDrawScreenSizeRate;

	//�Ԋu
	const float slotGaugeSpace = 30.0f;

	//�X���b�g�Q�[�W�ő傩��X���b�g�Q�[�W�̕`��T�C�Y���Z�o�iX�͗]�����l���j
	const Vec2<float>slotGaugeDrawSize =
	{
		(slotGaugeDrawScreenSize.x - (SLOT_GAUGE_MAX - 1) * slotGaugeSpace) / SLOT_GAUGE_MAX,
		slotGaugeDrawScreenSize.y,
	};

	//�`��ʒu��ԍ�
	const Vec2<float>minDrawPos = (slotGaugeScreenSize - slotGaugeDrawScreenSize) / 2.0f;

	//�X���b�g�Q�[�W�`��
	for (int i = 0; i < m_startSlotCount; ++i)
	{
		Vec2<float>drawPos =
		{
			minDrawPos.x + i * (slotGaugeDrawSize.x + slotGaugeSpace),
			minDrawPos.y
		};
		DrawFunc2D::DrawExtendGraph2D(drawPos, drawPos + slotGaugeDrawSize, m_slotGaugeTex);
	}
}

SlotMachine::SlotMachine()
{
	//�X���b�g�}�V������
	m_slotMachineObj = std::make_shared<ModelObject>("resource/user/model/", "slotMachine.glb");
	//�g�����X�t�H�[������
	m_slotMachineObj->m_transform.SetScale(2.0f);
	m_slotMachineObj->m_transform.SetPos({ 0.0f,3.0f,0.0f });

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

	/*--- �X���b�g�Q�[�W ---*/
	//�X���b�g�Q�[�W���f����ʃ����_�[�^�[�Q�b�g����
	m_slotGaugeScreen = D3D12App::Instance()->GenerateRenderTarget(
		D3D12App::Instance()->GetBackBuffFormat(),
		Color(70, 60, 94, 255), 
		Vec2<int>(1000,200),
		L"SlotGauge - Screen");
	//�X���b�g�Q�[�W�e�N�X�`������
	m_slotGaugeTex = D3D12App::Instance()->GenerateTextureBuffer(
		Color(74, 185, 163, 255));

	//�����_�[�^�[�Q�b�g���X���b�g�Q�[�W���f����ʃe�N�X�`���ɃA�^�b�`
	const std::string SLOT_GAUGE_SCREEN_MATERIAL_NAME = "SlotGaugeScreen";
	for (auto& mesh : m_slotMachineObj->m_model->m_meshes)
	{
		auto& material = mesh.material;
		if (material->name.compare(SLOT_GAUGE_SCREEN_MATERIAL_NAME) != 0)continue;

		//�ړI�̃}�e���A�������A�V�����e�N�X�`�����A�^�b�`
		material->texBuff[0] = m_slotGaugeScreen;
		break;
	}

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
	//���o�[������
	m_lever = REEL::NONE;

	//�X���b�g��]�\�񃊃Z�b�g
	m_startSlotCount = 0;

	//���[��������
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init();

	//�X���b�g�Q�[�W��ʍX�V
	UpdateSlotGaugeScreen();

	//�X���b�g�Q�[�W��������^�C���X�P�[��
	m_autoOperateTimeScale = 1.0f;
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
			//�X���b�g�Q�[�W��ʍX�V
			UpdateSlotGaugeScreen();
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

#include"BasicDraw.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	BasicDraw::Draw(*arg_lightMgr.lock(), m_slotMachineObj, *arg_cam.lock());

	//DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
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


	if (m_lever == REEL::NONE)return;
	//�X���b�g�Q�[�W��ʍX�V
	UpdateSlotGaugeScreen();
}

void SlotMachine::ReelSet(REEL arg_which,
	std::shared_ptr<TextureBuffer>& arg_reelTex,
	std::vector<ConstParameter::Slot::PATTERN>& arg_patternArray)
{
	m_reels[arg_which].SetPattern(arg_reelTex, arg_patternArray);
}
