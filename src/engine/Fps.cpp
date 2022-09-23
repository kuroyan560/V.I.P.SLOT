#include"Fps.h"
#include <profileapi.h>

static const float MILL_SECOND = 1.0 / 1000;

Fps::Fps(const int& FrameRate) : m_frameRate(FrameRate), m_minFrameTime(1.0f / (float)m_frameRate)
{
	// ���C�����[�v�ɓ���O�ɐ��x���擾���Ă���
	if (QueryPerformanceFrequency(&m_timeFreq) == FALSE) { // ���̊֐���0(FALSE)���A�鎞�͖��Ή�
		// ��������QueryPerformanceFrequency���g���Ȃ��l��(�Â�)PC�ł͂ǂ����F�X�L�c�C���낤��
	}
	// 1�x�擾���Ă���(����v�Z�p)
	QueryPerformanceCounter(&m_timeStart);
}

void Fps::Update()
{
	//Sleep(MILL_SECOND * 20);	//20ms�҂�

	// ���̎��Ԃ��擾
	QueryPerformanceCounter(&m_timeEnd);
	// (���̎��� - �O�t���[���̎���) / ���g�� = �o�ߎ���(�b�P��)
	m_frameTime = static_cast<float>(m_timeEnd.QuadPart - m_timeStart.QuadPart) / static_cast<float>(m_timeFreq.QuadPart);

	if (m_frameTime < m_minFrameTime) { // ���Ԃɗ]�T������
		// �~���b�ɕϊ�
		DWORD sleepTime = static_cast<DWORD>((m_minFrameTime - m_frameTime) * 1000);

		timeBeginPeriod(1); // ����\���グ��(�������Ȃ���Sleep�̐��x�̓K�^�K�^)
		Sleep(sleepTime);   // �Q��
		timeEndPeriod(1);   // �߂�

		// ���T�Ɏ����z��(�������Ȃ���fps���ςɂȂ�?)
	}

	if (m_frameTime > 0.0) { // �o�ߎ��Ԃ�0���傫��(�������Ȃ��Ɖ��̌v�Z�Ń[�����Z�ɂȂ�Ǝv���)
		m_fps = (m_fps * 0.99f) + (0.01f / m_frameTime); // ����fps���v�Z
		m_timeStart = m_timeEnd; // ����ւ�
	}
}