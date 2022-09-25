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

		//��]�ʁiUV�l��V�j
		float m_vOffset = 0.0f;

		//��]����
		bool m_isSpin = false;
		//��]���x
		float m_spinSpeed;

		//��]�n�߃t���O�i�����H�ō����x�ɂȂ�܂ł̊Ԃ�Stop�o���Ȃ��j
		bool m_isStartSpin = false;
		//���Ԍv��
		int m_timer;

		//��]��UV�l�ɔ��f
		void SpinAffectUV();
		
	public:
		//���[�����b�V���A�^�b�`
		void Attach(ModelMesh* ReelMesh);
		//������
		void Init(std::shared_ptr<TextureBuffer>ReelTex = nullptr);
		//�X�V�i��]�j
		void Update();

		//��]�X�^�[�g
		void Start();
	};
	std::array<Reel, REEL::NUM>m_reels;
	
public:
	SlotMachine();
	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};