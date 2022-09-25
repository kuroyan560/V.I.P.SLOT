#include "Reel.h"
#include"Model.h"
#include"ConstParameters.h"

void Reel::Attach(ModelMesh* ReelMesh)
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

void Reel::Init(std::shared_ptr<TextureBuffer> ReelTex, int PatternNum)
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

	//���[����~����V�I�t�Z�b�g�i���r���[�Ȉʒu�ɂȂ�Ȃ��悤�␳�����������́j
	m_vOffsetFixedStop = 0.0f;

	//�X���b�g�̊G������
	m_nowPatternIdx = 0;

	//�^�C�}�[���Z�b�g
	m_timer = -1;
}

void Reel::Update()
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

	//���[�����b�V���ɉ�]�𔽉f
	SpinAffectUV();
}

void Reel::Start()
{
	//��]�X�^�[�g
	m_isSpin = true;
	m_isStartSpin = true;
	m_isEndSpin = false;

	//�^�C�}�[�X�^�[�g
	m_timer = 0;

	//��]��
	m_nowPatternIdx = -1;

	//�U�����Z�b�g
	m_vOffset = m_vOffsetFixedStop;
}

void Reel::Stop()
{
	//��]�I��
	m_isEndSpin = true;

	//�^�C�}�[�X�^�[�g
	m_timer = 0;

	//������P�ʂ��ۂߍ���ŕ␳�i���r���[�Ȉʒu�Ŏ~�܂�Ȃ��悤�ɂ���j
	float tmp = roundf(m_vOffset * 10.0f) / 10.0f;

	//��~�ʒu�̊G���C���f�b�N�X�L�^
	const float vSpan = 1.0f / (m_patternNum - 1);
	m_nowPatternIdx = m_patternNum + static_cast<int>(tmp / vSpan);

	//�G���̃��[�v
	while (m_nowPatternIdx < 0)m_nowPatternIdx += m_patternNum;
	while (m_patternNum <= m_nowPatternIdx)m_nowPatternIdx -= m_patternNum;

	//��~�ʒu��V�l���L�^
	m_vOffsetFixedStop = 1.0f / m_patternNum * m_nowPatternIdx;

	printf("%f,%f,%d\n", m_vOffset, m_vOffsetFixedStop, m_nowPatternIdx);
}

void Reel::SpinAffectUV()
{
	//���_����UV��]�ʃZ�b�g
	for (int vertIdx = 0; vertIdx < static_cast<int>(m_meshPtr->mesh->vertices.size()); ++vertIdx)
	{
		m_meshPtr->mesh->vertices[vertIdx].uv.y = m_initV[vertIdx] + m_vOffset;
	}
	m_meshPtr->mesh->Mapping();
}
