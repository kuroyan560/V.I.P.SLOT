#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"Importer.h"
#include"GameCamera.h"

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

	//�R�C�����f���ǂݍ���
	m_coinModel = Importer::Instance()->LoadModel("resource/user/model/", "coin.glb");

	//�T�E���h�ǂݍ���
	m_spinStartSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_start.wav");
	m_reelStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/slot_stop.wav");
}

void SlotMachine::Init()
{
	//���[��������
	for (int reelIdx = 0; reelIdx < REEL::NUM; ++reelIdx)m_reels[reelIdx].Init();

	//���o�[������
	m_lever = -1;

	//�Ō�̃��[�����~�߂Ă���̎��Ԍv���p�^�C�}�[���Z�b�g
	m_slotWaitTimer.Reset(ConstParameter::Slot::SLOT_WAIT_TIME);

	//�R�C�����Z�b�g
	m_coinVault.Init(0);

	//BET���R�C�����Z�b�g
	m_betCoinArray.clear();
}

//�f�o�b�O�p
#include"UsersInput.h"

void SlotMachine::Update()
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
			m_lever++;
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

	for (auto& coin : m_betCoinArray)
	{
		if (coin.m_emitTimer.IsTimeUp())
		{
			//���肩��R�C���󂯎��
			assert(coin.m_otherVault); //�ꉞnull�`�F�b�N
			coin.m_otherVault->Pass(m_coinVault, coin.m_coinNum);
			//BET�ɐ���
			coin.m_bet = true;
		}

		//�^�C�}�[�v��
		coin.m_emitTimer.UpdateTimer();

		//�R�C���̍��W���Z�o���ăg�����X�t�H�[���ɓK�p
		Vec3<float>newPos = KuroMath::Lerp(coin.m_emitTransform.GetPos(), ConstParameter::Slot::COIN_PORT_POS, coin.m_emitTimer.GetTimeRate());
		coin.m_transform.SetPos(newPos);
	}

	//�������ɓ��B������폜
	m_betCoinArray.remove_if([](BetCoin& c)
		{
			return c.m_bet;
		});
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_megaPhoneObj, *arg_gameCam.lock()->GetBackCam(), 1.0f, AlphaBlendMode_None);

	//BET���ꂽ�R�C���̕`��
	for (auto& coin : m_betCoinArray)
	{
		DrawFunc3D::DrawNonShadingModel(m_coinModel, coin.m_transform, *arg_gameCam.lock()->GetFrontCam(), 1.0f, nullptr, AlphaBlendMode_None);
	}
}

void SlotMachine::Bet(CoinVault& arg_otherVault, int arg_coinNum, const Transform& arg_emitTransform)
{
	//BET���ꂽ�R�C�����ǉ�
	m_betCoinArray.emplace_front(&arg_otherVault, arg_coinNum, arg_emitTransform, ConstParameter::Slot::UNTIL_THROW_COIN_TO_BET);
}