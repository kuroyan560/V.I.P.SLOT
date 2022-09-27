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
	return true;
}

void SlotMachine::SlotPerform(const ConstParameter::Slot::PATTERN& Pattern)
{
	using namespace ConstParameter::Slot;
	using PATTERN = ConstParameter::Slot::PATTERN;

	//�R�C����Ԃ����o�i�X���b�g�㕔����R�C�����o�j
	bool returnCoinFlg = (Pattern == PATTERN::DOUBLE) || (Pattern == PATTERN::TRIPLE);

	if (returnCoinFlg)
	{
		//�X���b�g���̑��z
		int slotCoinNum = m_coinVault.GetNum();

		//�ԋp�R�C���̎���
		static const int RETURN_COIN_LIFE_TIME = 200;
		//�ԋp�R�C���̕��o�p���[����
		static const float RETURN_COIN_EMIT_POWER_MIN = 2.0f;
		//�ԋp�R�C���̕��o�p���[���
		static const float RETURN_COIN_EMIT_POWER_MAX = 6.0f;
		//�Ԋ҂̕��o�p���[XZ���ʕ����̋������[�g����
		static const float RETURN_COIN_EMIT_XZ_POWER_RATE_MIN = 0.05f;
		//�Ԋ҂̕��o�p���[XZ���ʕ����̋������[�g���
		static const float RETURN_COIN_EMIT_XZ_POWER_RATE_MAX = 0.2f;
		//�Ԋ҂̕��o�p���[Y�����̋������[�g����
		static const float RETURN_COIN_EMIT_Y_POWER_RATE_MIN = 0.3f;
		//�Ԋ҂̕��o�p���[Y�����̋������[�g���
		static const float RETURN_COIN_EMIT_Y_POWER_RATE_MAX = 1.0f;

		Transform initTransform;
		initTransform.SetPos(SLOT_POS_ON_BACK_CAM);

		//�`�悷��ԋp�R�C���̒ǉ�
		for (; RETURN_COIN_DRAW_NUM_PER <= slotCoinNum; slotCoinNum -= RETURN_COIN_DRAW_NUM_PER)
		{
			const float power = KuroFunc::GetRand(RETURN_COIN_EMIT_POWER_MIN, RETURN_COIN_EMIT_POWER_MAX);
			float vec_x = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
			vec_x *= KuroFunc::GetRandPlusMinus();
			float vec_y = KuroFunc::GetRand(RETURN_COIN_EMIT_Y_POWER_RATE_MIN, RETURN_COIN_EMIT_Y_POWER_RATE_MAX);
			float vec_z = KuroFunc::GetRand(RETURN_COIN_EMIT_XZ_POWER_RATE_MIN, RETURN_COIN_EMIT_XZ_POWER_RATE_MAX);
			vec_z *= KuroFunc::GetRandPlusMinus();

			const Vec3<float>initMove = Vec3<float>(vec_x, vec_y, vec_z) * power;

			m_returnCoinObjManager.Add(RETURN_COIN_DRAW_NUM_PER,
				initTransform, RETURN_COIN_LIFE_TIME, new ReturnCoinPerform(initMove));
		}
	}
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

	//�T�E���h�ǂݍ���
	m_spinStartSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_start.wav");
	m_reelStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_stop.wav");
	m_coinReturnSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav", 0.15f);
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
	m_betCoinObjManager.Init();

	//�ԋp�R�C�����Z�b�g
	m_returnCoinObjManager.Init();
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
			if (m_lever++ == REEL::RIGHT && CheckReelPattern())
			{
				//���ʂ��m�F���ĉ��o�̏���
				SlotPerform(m_reels[REEL::LEFT].GetNowPattern());
			}
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

	//BET�R�C���������ꉉ�o
	if (int betCoinNum = m_betCoinObjManager.Update())
	{
		//�v���C���[����R�C���󂯎��
		arg_playersVault.Pass(m_coinVault, betCoinNum);
		//���K�z���g�k
		m_megaPhoneExpandTimer.Reset(ConstParameter::Slot::MEGA_PHONE_EXPAND_TIME);
	}

	//�ԋp�R�C�����o
	if (int returnCoinNum = m_returnCoinObjManager.Update())
	{
		//�v���C���[�ɃR�C���ԋp
		m_coinVault.Pass(arg_playersVault, returnCoinNum);
		//SE�Đ�
		AudioApp::Instance()->PlayWave(m_coinReturnSE);
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
	m_betCoinObjManager.Draw(arg_lightMgr, arg_gameCam.lock()->GetFrontCam());
	//�ԋp�R�C���̕`��
	m_returnCoinObjManager.Draw(arg_lightMgr, arg_gameCam.lock()->GetBackCam());
}

void SlotMachine::Bet(int arg_coinNum, const Transform& arg_emitTransform)
{
	//BET���ꂽ�R�C�����ǉ�
	m_betCoinObjManager.Add(
		arg_coinNum, arg_emitTransform, ConstParameter::Slot::UNTIL_THROW_COIN_TO_BET, new BetCoinPerform());
}

void SlotMachine::BetCoinPerform::OnUpdate(Coins& arg_coin)
{
	//�R�C���̍��W���Z�o���ăg�����X�t�H�[���ɓK�p
	Vec3<float>newPos = KuroMath::Lerp(arg_coin.m_initTransform.GetPos(), ConstParameter::Slot::COIN_PORT_POS, arg_coin.m_timer.GetTimeRate());
	arg_coin.m_transform.SetPos(newPos);
}

void SlotMachine::ReturnCoinPerform::OnUpdate(Coins& arg_coin)
{
	const float GRAVITY = -0.002f;
	m_move.y += m_fallAccel;
	m_fallAccel += GRAVITY;

	auto pos = arg_coin.m_transform.GetPos();
	pos += m_move;

	//XZ���ʕ����̈ړ��ʂ͋�C��R�Ō���
	m_move.x = KuroMath::Lerp(m_move.x, 0.0f, 0.01f);
	m_move.z = KuroMath::Lerp(m_move.z, 0.0f, 0.01f);

	arg_coin.m_transform.SetPos(pos);
}
