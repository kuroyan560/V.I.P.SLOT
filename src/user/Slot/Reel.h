#pragma once
#include<vector>
#include<memory>
#include"ConstParameters.h"
#include"Timer.h"
class ModelMesh;
class TextureBuffer;

class Reel
{
private:
	using PATTERN = ConstParameter::Slot::PATTERN;

	//�Ώۂ̃��b�V���|�C���^
	ModelMesh* m_meshPtr = nullptr;
	//UV�l�̏������l
	std::vector<float>m_initV;

	//�G���z��
	std::vector<PATTERN>m_patternArray;

	//��]�ʁiUV�l��V�j
	float m_vOffset = 0.0f;

	//��]����
	bool m_isSpin = false;
	//��]���x
	float m_spinSpeed;

	//��]�n�߃t���O�i�����H�ō����x�ɂȂ�܂ł̊Ԃ�Stop�o���Ȃ��j
	bool m_isStartSpin = false;
	//��]�I���t���O
	bool m_isEndSpin = false;

	//���[����~����V�I�t�Z�b�g�i���r���[�Ȉʒu�ɂȂ�Ȃ��悤�␳�����������́j
	float m_vOffsetFixedStop;

	//���Ԍv��
	Timer m_timer;

	//�X���b�g���ʂ̊G���C���f�b�N�X
	int m_nowPatternIdx = 0;

	//��]��UV�l�ɔ��f
	void SpinAffectUV();

public:
	//���[�����b�V���A�^�b�`
	void Attach(ModelMesh* arg_reelMesh);
	//�G���Z�b�g
	void SetPattern(std::shared_ptr<TextureBuffer>arg_reelTex = nullptr, std::vector<PATTERN>arg_patternArray = {});

	//������
	void Init();
	//�X�V�i��]�j
	void Update(float arg_timeScale);

	//��]�X�^�[�g
	void Start();
	//��]�X�g�b�v
	void Stop();

	//��]��~���󂯕t���Ă��邩
	bool IsCanStop()const { return !m_isStartSpin; }

	//���݂̊G��
	const PATTERN& GetNowPattern()const
	{
		if (m_nowPatternIdx == -1)
		{
			printf("Now this reel is spining.");
			assert(0);
		}
		return m_patternArray[m_nowPatternIdx];
	}
};