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
		else if (m_reels[m_lever].IsCanStop())
		{
			m_reels[m_lever].Stop();
			AudioApp::Instance()->PlayWaveDelay(m_reelStopSE);
			m_lever++;
		}

		//�S���[����~��
		if (REEL::RIGHT < m_lever)m_lever = -1;
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

void SlotMachine::Reel::Init(std::shared_ptr<TextureBuffer> ReelTex)
{
	//���[���̃��b�V�����������Ă��Ȃ�
	if (m_meshPtr == nullptr)
	{
		printf("The reel hasn't found its mesh pointer.It is nullptr.");
		assert(0);
	}

	//���[���̃e�N�X�`���w��
	if (ReelTex)m_meshPtr->material->texBuff[COLOR_TEX] = ReelTex;

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
			m_timer = -1;	//�^�C�}�[���Z�b�g
		}
	}

	//V��]
	m_vOffset += m_spinSpeed;

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
}

void SlotMachine::Reel::Stop()
{
	m_isSpin = false;
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