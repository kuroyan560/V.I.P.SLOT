#pragma once
#include<memory>
class TimeScale;
class Camera;

//�C�x���g�N���X
class Event
{
	//�i�s���̃C�x���g
	static Event* s_nowEvent;
	//���̃C�x���g
	static Event* s_nextEvent;

protected:
	//�C�x���g���̃p�����[�^��ݒ�
	Event() {}

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
	void SetNextEvent()
	{
		if (s_nextEvent != nullptr)return;
		s_nextEvent = this;
		OnStart();
	}

	//�C�x���g�̏�����
	static void StaticInit() 
	{ 
		s_nowEvent = nullptr; 
		s_nextEvent = nullptr;
	}

	//�i�s���̃C�x���g�̍X�V
	static void StaticUpdate()
	{
		//�C�x���g�i�s���łȂ�
		if (s_nowEvent == nullptr)
		{
			//���̃C�x���g���Z�b�g����Ă���
			if (s_nextEvent != nullptr)
			{
				//���̃C�x���g�J�n
				s_nowEvent = s_nextEvent;
				s_nowEvent->OnStart();
				s_nextEvent = nullptr;
			}
			return;
		}

		s_nowEvent->Update();

		if (s_nowEvent->End()) 
		{
			s_nowEvent->OnFinish();
			s_nowEvent = nullptr;
		}
	}

	//�C�x���g���i�s����
	bool IsProceed()const
	{
		return s_nowEvent != nullptr && s_nowEvent == this;
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