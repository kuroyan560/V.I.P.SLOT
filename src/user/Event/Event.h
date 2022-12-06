#pragma once
#include<memory>
class TimeScale;
class Camera;

//�C�x���g�N���X
class Event
{
	//�i�s���̃C�x���g
	static Event* s_nowEvent;

	//�C�x���g�i�s���̃p�����[�^
	//�Փ˔���t���O
	const bool m_collision;

protected:
	//�C�x���g���̃p�����[�^��ݒ�
	Event(bool arg_collision) :m_collision(arg_collision) {}

	//�C�x���g�J�n���̏���
	virtual void OnStart() = 0;
	//�C�x���g�̍X�V����
	virtual void OnUpdate() = 0;
	//�C�x���g�I�����̏���
	virtual void OnFinish() = 0;
	//�C�x���g�I������
	virtual bool End() = 0;
	//��p�̃��C���i�ߌi�j�C�x���g�J�����Q�b�^�i�f�t�H���g�F�Ȃ��j
	virtual std::shared_ptr<Camera> GetMainCam() { return nullptr; }
	//��p�̃T�u�i���i�j�C�x���g�J�����Q�b�^�i�f�t�H���g�F�Ȃ��j
	virtual std::shared_ptr<Camera> GetSubCam() { return nullptr; }

	void Update()
	{
		OnUpdate();
	}

public:
	virtual ~Event() {}
	//�C�x���g�J�n
	void Start()
	{
		if (s_nowEvent != nullptr)return;
		s_nowEvent = this;
		OnStart();
	}

	//�C�x���g�̏�����
	static void StaticInit() { s_nowEvent = nullptr; }

	//�i�s���̃C�x���g�̍X�V
	static void StaticUpdate()
	{
		if (s_nowEvent == nullptr)return;

		s_nowEvent->Update();

		if (s_nowEvent->End()) 
		{
			s_nowEvent->OnFinish();
			s_nowEvent = nullptr;
		}
	}

	//�����蔻����s����
	static bool CollisionFlg() 
	{
		if (s_nowEvent == nullptr)return true;
		return s_nowEvent->m_collision;
	}

	//���C���i�ߌi�j�C�x���g�J�����Q�b�^
	static std::shared_ptr<Camera>GetMainEventCamera()
	{
		if (s_nowEvent == nullptr)return nullptr;
		return s_nowEvent->GetMainCam();
	}
	//�T�u�i���i�j�C�x���g�J�����Q�b�^
	static std::shared_ptr<Camera>GetSubEventCamera()
	{
		if (s_nowEvent == nullptr)return nullptr;
		return s_nowEvent->GetSubCam();
	}
};