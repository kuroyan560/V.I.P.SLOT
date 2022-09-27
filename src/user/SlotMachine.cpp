#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"Importer.h"
#include"GameCamera.h"

bool SlotMachine::CheckReelPattern()
{
	const auto& pattern = m_reels[REEL::LEFT].GetNowPattern();

	//�G�����Ⴆ�Ή��������I��
	if (pattern != m_reels[REEL::CENTER].GetNowPattern())return false;
	if (pattern != m_reels[REEL::RIGHT].GetNowPattern())return false;

	m_patternMgr.Invoke(pattern);
}

SlotMachine::SlotMachine() : m_patternMgr(m_coinVault)
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

	//BET�R�C���}�l�[�W��
	m_betCoinObjManager = std::make_unique<CoinObjectManager>(new BetCoinPerform());

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
		if (i % 2 == 0)testPatternArray[i] = PATTERN::DOUBLE;
		else testPatternArray[i] = PATTERN::TRIPLE;
	}

	//���[��������
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init(nullptr, testPatternArray);

	//���o�[������
	m_lever = -1;

	//�Ō�̃��[�����~�߂Ă���̎��Ԍv���p�^�C�}�[���Z�b�g
	m_slotWaitTimer.Reset(ConstParameter::Slot::SLOT_WAIT_TIME);

	//�R�C�����Z�b�g
	m_coinVault.Set(0);

	//BET�R�C�����Z�b�g
	m_betCoinObjManager->Init();
}

//�f�o�b�O�p
#include"UsersInput.h"

void SlotMachine::Update(CoinVault& arg_playersVault)
{
	//���[���X�V
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Update();

	//���o�[����
	if (UsersInput::Instance()->ControllerOnTrigger(0, XBOX_BUTTON::RB))
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
			if (m_lever++ == REEL::RIGHT)CheckReelPattern();
		}
	}

	//�S���[����~��
	if (REEL::RIGHT < m_lever)
	{
		//���̃X���b�g��]�\��
		if (m_slotWaitTimer.UpdateTimer())
		{
			m_lever = -1;
			m_slotWaitTimer.Reset();
		}
	}

	const int MEGA_PHONE_EXPAND_TIME = 60;

	//BET�R�C���������ꉉ�o
	if (int betCoinNum = m_betCoinObjManager->Update())
	{
		//�v���C���[����R�C���󂯎��
		arg_playersVault.Pass(m_coinVault, betCoinNum);
		//���K�z���g�k
		m_megaPhoneExpandTimer.Reset(MEGA_PHONE_EXPAND_TIME);
	}

	//���K�z���g�k
	float megaPhoneScale = KuroMath::Ease(Out, Elastic, m_megaPhoneExpandTimer.GetTimeRate(), 
		ConstParameter::Slot::MEGA_PHONE_EXPAND_SCALE, 1.0f);
	m_megaPhoneObj->m_transform.SetScale(megaPhoneScale);
	m_megaPhoneExpandTimer.UpdateTimer();
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_megaPhoneObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);

	//BET���ꂽ�R�C���̕`��
	m_betCoinObjManager->Draw(arg_lightMgr, arg_gameCam);
}

void SlotMachine::Bet(int arg_coinNum, const Transform& arg_emitTransform)
{
	//BET���ꂽ�R�C�����ǉ�
	m_betCoinObjManager->Add(arg_coinNum, arg_emitTransform, ConstParameter::Slot::UNTIL_THROW_COIN_TO_BET);
}

void SlotMachine::BetCoinPerform::OnUpdate(Coins& arg_coin)
{
	//�R�C���̍��W���Z�o���ăg�����X�t�H�[���ɓK�p
	Vec3<float>newPos = KuroMath::Lerp(arg_coin.m_initTransform.GetPos(), ConstParameter::Slot::COIN_PORT_POS, arg_coin.m_timer.GetTimeRate());
	arg_coin.m_transform.SetPos(newPos);
}