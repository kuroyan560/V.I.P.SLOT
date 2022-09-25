#pragma once
#include<memory>
#include<array>
#include<vector>
#include<string>
class ModelObject;
class LightManager;
class Camera;
class TextureBuffer;
class ModelMesh;

class SlotMachine
{
	//�X���b�g�}�V��
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//���[��
	enum REEL { LEFT, CENTER, RIGHT, NUM };
	const std::array<std::string, REEL::NUM>REEL_MATERIAL_NAME =
	{
		"Reel_Left","Reel_Center","Reel_Right"
	};
	class Reel
	{
	private:
		//�Ώۂ̃��b�V���|�C���^
		ModelMesh* m_meshPtr = nullptr;
		//UV�l�̏������l
		std::vector<float>m_initV;

		//�G���̐�
		int m_patternNum = 20;

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
		int m_timer;

		//�X���b�g���ʂ̊G���C���f�b�N�X
		int m_nowPatternIdx = 0;

		//��]��UV�l�ɔ��f
		void SpinAffectUV();
		
	public:
		//���[�����b�V���A�^�b�`
		void Attach(ModelMesh* ReelMesh);
		//������
		void Init(std::shared_ptr<TextureBuffer>ReelTex = nullptr, int PatternNum = 20);
		//�X�V�i��]�j
		void Update();

		//��]�X�^�[�g
		void Start();
		//��]�X�g�b�v
		void Stop();

		//��]��~���󂯕t���Ă��邩
		bool IsCanStop()const { return !m_isStartSpin; }

		//��]����
		const bool& IsSpin()const { return m_isSpin; }
	};
	std::array<Reel, REEL::NUM>m_reels;

	//�X���b�g�̃��o�[�t���O�iStart��Stop���j
	int m_lever;

	//�T�E���h
	int m_spinStartSE;	//��]�X�^�[�g
	int m_reelStopSE;		//���[���X�g�b�v
	
public:
	SlotMachine();
	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};