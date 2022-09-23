#include "AudioApp.h"

AudioApp* AudioApp::s_instance = nullptr;

using namespace std;

AudioApp::AudioApp()
{

	if (s_instance != nullptr)
	{
		printf("���ɃC���X�^���X������܂��BAudioApp�͂P�̃C���X�^���X�������Ă܂���\n");
		assert(0);
	}
	s_instance = this;

	HRESULT result;

	//XAudio�G���W���̃C���X�^���X�𐶐�
	result = XAudio2Create(&m_xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);

	//�}�X�^�[�{�C�X�𐶐�
	result = m_xAudio2->CreateMasteringVoice(&m_masterVoice);
}

AudioApp::~AudioApp()
{
	m_xAudio2.Reset();
	for (auto itr = m_audios.begin(); itr != m_audios.end(); ++itr)
	{
		itr->Unload();
	}
}

void AudioApp::Update()
{
	for (auto& audio : m_audios)
	{
		audio.m_playTrigger = false;
	}

	if (!m_playHandleArray.empty())
	{
		for (auto& audioArray : m_playHandleArray)
		{
			if (NowPlay(audioArray.m_handles[audioArray.m_nowIdx]))
			{
				audioArray.m_nowIdx++;

				if (audioArray.m_nowIdx < audioArray.m_handles.size())
				{
					PlayWave(audioArray.m_handles[audioArray.m_nowIdx]);
				}
			}
		}

		//�S���Đ�����������폜
		for (auto itr = m_playHandleArray.begin(); itr != m_playHandleArray.end();)
		{
			if (itr->m_handles.size() <= itr->m_nowIdx)
			{
				itr = m_playHandleArray.erase(itr);
			}
			else ++itr;
		}
	}

	//�x���Đ�
	for (auto& info : m_delayAudioHandle)
	{
		info.m_delayFrame--;
		if (!info.m_delayFrame)PlayWave(info.m_handle);
	}
	m_delayAudioHandle.remove_if([](PlayAudioDelay& info)
		{
			return !info.m_delayFrame;
		});
}

bool AudioApp::NowPlay(const int& Handle)
{
	auto itr = m_audios.begin();
	for (int i = 0; i < Handle; i++)
	{
		itr++;
	}

	if (itr->m_pSourceVoice != nullptr)
	{
		XAUDIO2_VOICE_STATE state;
		itr->m_pSourceVoice->GetState(&state);
		return !(state.pCurrentBufferContext == nullptr);
	}
	else return false;
}

void AudioApp::ChangeVolume(const int& Handle, float Volume)
{
	auto itr = m_audios.begin();
	for (int i = 0; i < Handle; i++)
	{
		itr++;
	}
	itr->m_volume = Volume;
	if (itr->m_pSourceVoice != nullptr)
	{
		itr->m_pSourceVoice->SetVolume(itr->m_volume);
	}
}

float AudioApp::GetVolume(const int& Handle)
{
	auto itr = m_audios.begin();
	for (int i = 0; i < Handle; i++)
	{
		itr++;
	}
	return itr->m_volume;
}

int AudioApp::LoadAudio(string FileName, const float& Volume)
{
	//if (!audios.empty())
	//{
	//	int i = 0;
	//	for (auto itr = audios.begin(); itr != audios.end(); itr++)
	//	{
	//		if (itr->filePass == FileName)
	//		{
	//			return i;
	//		}
	//		i++;
	//	}
	//}

	//audios.emplace_back(FileName);

	//HRESULT result;

	////�@�t�@�C���I�[�v��
	//	//�t�@�C�����̓X�g���[���̃C���X�^���X
	//std::ifstream file;
	////.wav�t�@�C�����o�C�i�����[�h�ŊJ��
	//file.open(FileName.c_str(), std::ios_base::binary);
	////�t�@�C���I�[�v�����s�����o����
	//if (file.fail())
	//{
	//	assert(0);
	//}

	////�A.wav�f�[�^���
	////RITF�w�b�_�[�̓ǂݍ���
	//file.read((char*)&audios.back().riff, sizeof(audios.back().riff));
	////�t�@�C����RIFF���`�F�b�N
	//if (strncmp(audios.back().riff.chunk.id, "RIFF", 4) != 0)
	//{
	//	assert(0);
	//}

	////Format�`�����N�̓ǂݍ���
	//file.read((char*)&audios.back().format, sizeof(audios.back().format));
	////�t�@�C����Format���`�F�b�N
	//if (strncmp(audios.back().format.chunk.id, "fmt", 3) != 0)
	//{
	//	assert(0);
	//}

	////Data�`�����N�̓ǂݍ���
	//file.read((char*)&audios.back().data, sizeof(audios.back().data));
	////�t�@�C����Data���`�F�b�N
	//if (strncmp(audios.back().data.id, "data", 4) != 0)
	//{
	//	assert(0);
	//}

	////Data�`�����N�̃f�[�^���i�g�`�f�[�^�j�̓ǂݍ���
	//audios.back().pBuffer = new char[audios.back().data.size];
	//file.read(audios.back().pBuffer, audios.back().data.size);

	////Wave�t�@�C�������
	//file.close();

	//return audios.size() - 1;
	if (!m_audios.empty())
	{
		int i = 0;
		for (auto itr = m_audios.begin(); itr != m_audios.end(); itr++)
		{
			if (itr->m_filePass == FileName)
			{
				return i;
			}
			i++;
		}
	}

	m_audios.emplace_back(FileName);

	HRESULT result;

	//�@�t�@�C���I�[�v��
		//�t�@�C�����̓X�g���[���̃C���X�^���X
	std::ifstream file;
	//.wav�t�@�C�����o�C�i�����[�h�ŊJ��
	file.open(FileName.c_str(), std::ios_base::binary);
	//�t�@�C���I�[�v�����s�����o����
	if (file.fail())
	{
		assert(0);
	}

	while (1)
	{
		Chunk chunk;
		file.read((char*)&chunk, sizeof(Chunk));

		if (strncmp(chunk.m_id, "RIFF", 4) == 0)
		{
			m_audios.back().m_riff.m_chunk = chunk;
			file.read(m_audios.back().m_riff.m_type, sizeof(m_audios.back().m_riff.m_type));
		}
		else if (strncmp(chunk.m_id, "fmt ", 4) == 0)
		{
			m_audios.back().m_format.m_chunk = chunk;
			file.read((char*)&m_audios.back().m_format.m_fmt, chunk.m_size);
		}
		else if (strncmp(chunk.m_id, "data", 4) == 0)
		{
			m_audios.back().m_data = chunk;
			//Data�`�����N�̃f�[�^���i�g�`�f�[�^�j�̓ǂݍ���
			m_audios.back().m_pBuffer = new char[m_audios.back().m_data.m_size];
			file.read(m_audios.back().m_pBuffer, m_audios.back().m_data.m_size);
			break;
		}
		else
		{
			//�s�K�v�ȏ��Ȃ疳��
			file.seekg(chunk.m_size, ios::cur);
		}
	}

	//Wave�t�@�C�������
	file.close();

	int handle = m_audios.size() - 1;

	ChangeVolume(handle, Volume);

	return handle;
}

int AudioApp::PlayWave(const int& Handle, bool LoopFlag)
{
	HRESULT result;

	auto itr = m_audios.begin();
	for (int i = 0; i < Handle; i++)
	{
		itr++;
	}

	//�����ɓ����������Đ����Ȃ�
	if (itr->m_playTrigger)
	{
		return -1;
	}

	//���[�v�Đ��ōĐ����悤�Ƃ�������ɗ���Ă�����̂��~
	if (LoopFlag && NowPlay(Handle))
	{
		itr->m_pSourceVoice->Stop();
	}

//�B�T�E���h�Đ�
	WAVEFORMATEX wfex{};
	//�g�`�t�H�[�}�b�g�̐ݒ�
	memcpy(&wfex, &itr->m_format.m_fmt, sizeof(itr->m_format.m_fmt));
	wfex.wBitsPerSample = itr->m_format.m_fmt.nBlockAlign * 8 / itr->m_format.m_fmt.nChannels;

	//�g�`�t�H�[�}�b�g������SourceVoice�̐���
	result = m_xAudio2->CreateSourceVoice(&itr->m_pSourceVoice, &wfex, 0, 2.0f, &m_voiceCallback);
	if FAILED(result)
	{
		return -1;
	}
	//�Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = (BYTE*)itr->m_pBuffer;
	buf.pContext = itr->m_pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.AudioBytes = itr->m_data.m_size;
	if (LoopFlag)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	
	//���ʂ̐ݒ�
	itr->m_pSourceVoice->SetVolume(itr->m_volume);

	//�g�`�f�[�^�̍Đ�
	result = itr->m_pSourceVoice->SubmitSourceBuffer(&buf);
	result = itr->m_pSourceVoice->Start();

	itr->m_playTrigger = true;
}

void AudioApp::StopWave(const int& Handle)
{
	auto itr = m_audios.begin();
	for (int i = 0; i < Handle; i++)
	{
		itr++;
	}
	if (itr->m_pSourceVoice != nullptr)
	{
		itr->m_pSourceVoice->Stop();
		itr->m_pSourceVoice->FlushSourceBuffers();
	}
}

void AudioApp::AudioData::Unload()
{
	if (m_pBuffer != nullptr)delete[] m_pBuffer;
}
