#include "SlotMachine.h"
#include"Object.h"
#include"Model.h"
#include"ConstParameters.h"
#include"AudioApp.h"

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

		//�S���[����~��
		if (REEL::RIGHT < m_lever && !m_reels[REEL::RIGHT].IsSpin())m_lever = -1;
	}
}

#include"DrawFunc3D.h"
void SlotMachine::Draw(std::weak_ptr<LightManager> LigMgr, std::weak_ptr<Camera> Cam)
{
	DrawFunc3D::DrawNonShadingModel(m_slotMachineObj, *Cam.lock(), 1.0f, AlphaBlendMode_None);
}

void SlotMachine::Reel::Attach(ModelMesh* ReelMesh)
{
	m_meshPtr = ReelMesh;

	//���[�����b�V�����_��V�������l�ۑ�
	auto& vertices = m_meshPtr->mesh->vertices;
	m_initV.resize(vertices.size());
	for (int vertIdx = 0; vertIdx < static_cast<int>(vertices.size()); ++vertIdx)
	{
		m_initV[vertIdx] = vertices[vertIdx].uv.y;
	}
}

void SlotMachine::Reel::Init(std::shared_ptr<TextureBuffer> ReelTex, int PatternNum)
{
	//���[���̃��b�V�����������Ă��Ȃ�
	if (m_meshPtr == nullptr)
	{
		printf("The reel hasn't found its mesh pointer.It is nullptr.");
		assert(0);
	}

	//���[���̃e�N�X�`���w��
	if (ReelTex)m_meshPtr->material->texBuff[COLOR_TEX] = ReelTex;

	//�G���̐��w��
	m_patternNum = PatternNum;

	//��]�ʃ��Z�b�g
	m_vOffset = 0.0f;
	//���[�����b�V���ɔ��f
	SpinAffectUV();

	//��]�t���O���Z�b�g
	m_isSpin = false;
	//��]���x���Z�b�g
	m_spinSpeed = 0.0f;

	//��]�n�߃t���O���Z�b�g
	m_isStartSpin = false;
	//��]�I���t���O���Z�b�g
	m_isEndSpin = false;

	//�X���b�g�̊G������
	m_nowPatternIdx = 0;

	//�^�C�}�[���Z�b�g
	m_timer = -1;
}

void SlotMachine::Reel::Update()
{
	//��]������Ȃ��Ȃ�X���[
	if (!m_isSpin)return;

	//�^�C�}�[�N�����i -1 �ŃI�t�j
	if (0 <= m_timer)m_timer++;

	//��]�n��
	if (m_isStartSpin)
	{
		//��]���x����
		m_spinSpeed = KuroMath::Ease(In, Back, m_timer,
			ConstParameter::Slot::UNTIL_MAX_SPEED_TIME, 0.0f, ConstParameter::Slot::MAX_SPIN_SPEED);

		//�ō����x���B
		if (ConstParameter::Slot::UNTIL_MAX_SPEED_TIME < m_timer)
		{
			m_isStartSpin = false;	//��]�n�߃t���O�����낷
			m_timer = -1;				//�^�C�}�[���Z�b�g
		}
	}
	
	//��]�I��
	if (m_isEndSpin)
	{
		// (0 ~ 1) �� (-1 ~1) �͈̔͂ɕ␳���ăC�[�W���O�v�Z
		float easeRate = KuroMath::Ease(Out, Elastic, m_timer, 
			ConstParameter::Slot::FINISH_SPIN_TIME, 0.0f, 1.0f) * 2.0f - 2.0f;

		//��]��~����̐U���ʌv�Z
		float shake = ConstParameter::Slot::FINISH_SPIN_SHAKE_MAX * easeRate;

		//�U�����Ȃ����~
		m_vOffset = m_vOffsetFixedStop + shake;

		//�U���I��
		if (ConstParameter::Slot::FINISH_SPIN_TIME < m_timer)
		{
			m_isSpin = false;			//��]�I��
			m_isEndSpin = false;	//��]�I���t���O�����낷
			m_timer = -1;				//�^�C�}�[���Z�b�g
		}
	}
	//V��]
	else
	{
		m_vOffset += m_spinSpeed;
	}

	//���[�v
	if (m_vOffset < -1.0f)m_vOffset += 1.0f;

	//���[�����b�V���ɉ�]�𔽉f
	SpinAffectUV();
}

void SlotMachine::Reel::Start()
{
	//��]�X�^�[�g
	m_isSpin = true;
	m_isStartSpin = true;

	//�^�C�}�[�X�^�[�g
	m_timer = 0;

	//��]��
	m_nowPatternIdx = -1;
}

void SlotMachine::Reel::Stop()
{
	//��]�I��
	m_isEndSpin = true;

	//�^�C�}�[�X�^�[�g
	m_timer = 0;

	//������P�ʂ��ۂߍ���
	m_vOffsetFixedStop = roundf(m_vOffset * 10.0f) / 10.0f;

	//��~�ʒu�̊G���C���f�b�N�X�L�^
	const float vSpan = 1.0f / (m_patternNum - 1);
	m_nowPatternIdx = m_patternNum + static_cast<int>(m_vOffsetFixedStop / vSpan);
	printf("\n%d", m_nowPatternIdx);
}

void SlotMachine::Reel::SpinAffectUV()
{
	//���_����UV��]�ʃZ�b�g
	for (int vertIdx = 0; vertIdx < static_cast<int>(m_meshPtr->mesh->vertices.size()); ++vertIdx)
	{
		m_meshPtr->mesh->vertices[vertIdx].uv.y = m_initV[vertIdx] + m_vOffset;
	}
	m_meshPtr->mesh->Mapping();
}
