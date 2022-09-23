#pragma once
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")

#include<wrl.h>

#include<fstream>
#include<assert.h>

#include<list>

#include<string>

#include<vector>
#include<forward_list>

class AudioApp
{
private:
	static AudioApp* s_instance;
public:
	static AudioApp* Instance()
	{
		if (s_instance == nullptr)
		{
			printf("AudioApp�̃C���X�^���X���Ăяo�����Ƃ��܂�����nullptr�ł���\n");
			assert(0);
		}
		return s_instance;
	}
private:
	//�`�����N�w�b�_
	struct Chunk
	{
		char m_id[4];		//�`�����N����ID
		int32_t m_size;	//�`�����N�̃T�C�Y
	};

	//RTFF�w�b�_�`�����N
	struct RiffHeader
	{
		Chunk m_chunk;	//"RIFF"
		char m_type[4];	//"WAVE"
	};

	//FMT�`�����N
	struct FormatChunck
	{
		Chunk m_chunk;	//"fmt "
		WAVEFORMAT m_fmt;	//�g�`�t�H�[�}�b�g
	};

	Microsoft::WRL::ComPtr<IXAudio2>m_xAudio2;
	IXAudio2MasteringVoice* m_masterVoice;

	class XAudio2VoiceCallback : public IXAudio2VoiceCallback
	{
	public:
		virtual ~XAudio2VoiceCallback() {};
		//�{�C�X�����p�X�̊J�n��
		STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired) {};
		//�{�C�X�����p�X�̏I����
		STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS) {};
		//�o�b�t�@�X�g���[���̍Đ����I�������Ƃ�
		STDMETHOD_(void, OnStreamEnd) (THIS) {};
		//�o�b�t�@�̎g�p�J�n��
		STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext) {};
		//�o�b�t�@�̖����ɒB������
		STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext){	};
		//�Đ������[�v�ʒu�ɒB������
		STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext) {};
		//�{�C�X�̎��s�G���[��
		STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error) {};
	};
	XAudio2VoiceCallback m_voiceCallback;

	class AudioData
	{
	public:
		std::string m_filePass;
		RiffHeader m_riff;
		FormatChunck m_format;
		Chunk m_data;
		char* m_pBuffer = nullptr;
		IXAudio2SourceVoice* m_pSourceVoice = nullptr;
		float m_volume = 1.0f;
		bool m_loop = false;
		bool m_playTrigger = false;

		AudioData(std::string FilePass)
			:m_filePass(FilePass) {};
		void Unload();
	};

	struct PlayAudioArray
	{
		std::vector<int>m_handles;
		int m_nowIdx = 0;
		PlayAudioArray(const std::vector<int>& Handles) :m_handles(Handles) {}
	};

	struct PlayAudioDelay
	{
		int m_handle;
		int m_delayFrame;
		PlayAudioDelay(const int& Handle, const int& DelayFrame = 10)
			:m_handle(Handle), m_delayFrame(DelayFrame) {}
	};

	std::list<AudioData>m_audios;
	std::vector<PlayAudioArray>m_playHandleArray;
	std::forward_list<PlayAudioDelay>m_delayAudioHandle;

public:
	AudioApp();
	~AudioApp();

	void Update();
	bool NowPlay(const int& Handle);
	void ChangeVolume(const int& Handle, float Volume);
	float GetVolume(const int& Handle);
	int LoadAudio(std::string FileName, const float& Volume = 1.0f);
	int PlayWave(const int& Handle, bool LoopFlag = false);
	void PlayWaveDelay(const int& Handle, const int& DelayFrame = 10)
	{
		m_delayAudioHandle.push_front(PlayAudioDelay(Handle, DelayFrame));
	}
	int PlayWaveArray(const std::vector<int>& Handles)	//�����̉����𓯃t���[���ōĐ����Ȃ��悤�A���ԂɍĐ�
	{
		if (Handles.empty())return 0;
		m_playHandleArray.emplace_back(Handles);
		return PlayWave(Handles[0]);
	}
	void StopWave(const int& Handle);
};