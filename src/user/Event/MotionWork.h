#pragma once
#include"Vec.h"
#include"KuroMath.h"
#include<vector>
#include"Timer.h"

//2D�p�����̐���P�ʃ��[�V����
struct Motion2D
{
	//���W�J�n�l
	Vec2<float>m_startPos = { 0.0f,0.0f };
	//���W�I���l
	Vec2<float>m_endPos = { 0.0f,0.0f };

	//�C�[�W���O�p�����[�^
	EasingParameter m_easeParam = { Out,Exp };

	//�����鎞��
	float m_interval;
};

//3D�p�����̐���P�ʃ��[�V����
struct Motion3D
{
	//���W�J�n�l
	Vec3<float>m_startPos = { 0.0f,0.0f,0.0f };
	//���W�I���l
	Vec3<float>m_endPos = { 0.0f,0.0f,0.0f };

	//�����_�̊J�n�l
	Vec3<float>m_startTarget = { 0.0f,0.0f,1.0f };
	//�����_�̊J�n�l
	Vec3<float>m_endTarget = { 0.0f,0.0f,-1.0f };

	//�C�[�W���O�p�����[�^
	EasingParameter m_easeParam = { Out,Exp };

	//�����鎞��
	float m_interval;
};

//2D�����̐���i�J�������[�N�Ȃǁj
class MotionWork2D
{
public:
	//�����̔z��
	std::vector<Motion2D>m_motions;
	//���ݐi�s���̃��[�V�����C���f�b�N�X
	int m_motionWorkIdx;
	//���Ԍv��
	Timer m_timer;

	Vec2<float>m_nowPos;
	bool m_auto;

public:
	/// <summary>
	/// ���[�V�����X�V
	/// </summary>
	/// <param name="arg_timeScale">�^�C���X�P�[��</param>
	/// <returns>���݂̃��[�V�������I��������</returns>
	bool Update(float arg_timeScale);

	/// <summary>
	/// ���[�V�������[�N�X�^�[�g
	/// </summary>
	/// <param name="arg_auto">�����Ŏ��̃��[�V�����ֈڍs���邩</param>
	void Start(bool arg_auto);

	//�O�����瓮�I�Ɏ��̃��[�V�����֐i��
	void Proceed();

	//�I�����Ă��邩
	bool IsEnd()
	{
		return m_timer.IsTimeUp() && static_cast<int>(m_motions.size() - 1) <= m_motionWorkIdx;
	}
	//�Ō�̃��[�V������
	bool IsLastMotion()const
	{
		return static_cast<int>(m_motions.size()) - 1 <= m_motionWorkIdx;
	}

	//���[�V�����ǉ�
	void Add(Motion2D& arg_motion)
	{
		m_motions.emplace_back(arg_motion);
	}

	//�w�肵���C���f�b�N�X�̃��[�V�����ւ̎Q�Ǝ擾
	Motion2D& GetMotion(int arg_motionWorkIdx)
	{
		assert(0 <= arg_motionWorkIdx && arg_motionWorkIdx < static_cast<int>(m_motions.size()));
		return m_motions[arg_motionWorkIdx];
	}

	//���[�V�������[�N�S�̂ɂ����鎞��
	float GetFullInterval()
	{
		float interval = 0.0f;
		for (auto& m : m_motions)interval += m.m_interval;
		return interval;
	}

	//�Q�b�^
	const Vec2<float>& GetPos()const { return m_nowPos; }
	float GetTimeRate() { return m_timer.GetTimeRate(); }

	/// <summary>
	/// Imgui�f�o�b�O�ō��ڒǉ�
	/// </summary>
	/// <param name="arg_tag">���[�V�����Ɋւ���p�����[�^�ݒ�̍ۂɃ��x���ɕt�^���閼�O</param>
	/// <param name="arg_changePos">�ω������������W�̕ύX��̒l�i�[��</param>
	void ImguiDebugItems(
		std::string arg_tag,
		const Vec2<float>* arg_changePos = nullptr);
};

//3D�����̐���i�J�������[�N�Ȃǁj
class MotionWork3D
{
public:
	//�����̔z��
	std::vector<Motion3D>m_motions;
	//���ݐi�s���̃��[�V�����C���f�b�N�X
	int m_motionWorkIdx;
	//���Ԍv��
	Timer m_timer;

	Vec3<float>m_nowPos;
	Vec3<float>m_nowTarget;
	bool m_auto;

public:
	/// <summary>
	/// ���[�V�����X�V
	/// </summary>
	/// <param name="arg_timeScale">�^�C���X�P�[��</param>
	/// <returns>���݂̃��[�V�������I��������</returns>
	bool Update(float arg_timeScale);

	/// <summary>
	/// ���[�V�������[�N�X�^�[�g
	/// </summary>
	/// <param name="arg_auto">�����Ŏ��̃��[�V�����ֈڍs���邩</param>
	void Start(bool arg_auto);

	//�O�����瓮�I�Ɏ��̃��[�V�����֐i��
	void Proceed();

	//�I�����Ă��邩
	bool IsEnd()
	{
		return m_timer.IsTimeUp() && static_cast<int>(m_motions.size() - 1) <= m_motionWorkIdx;
	}
	//�Ō�̃��[�V������
	bool IsLastMotion()const
	{
		return static_cast<int>(m_motions.size()) - 1 <= m_motionWorkIdx;
	}

	//���[�V�����ǉ�
	void Add(Motion3D& arg_motion)
	{
		m_motions.emplace_back(arg_motion);
	}

	//�w�肵���C���f�b�N�X�̃��[�V�����ւ̎Q�Ǝ擾
	Motion3D& GetMotion(int arg_motionWorkIdx)
	{
		assert(0 <= arg_motionWorkIdx && arg_motionWorkIdx < static_cast<int>(m_motions.size()));
		return m_motions[arg_motionWorkIdx];
	}

	//�Q�b�^
	const Vec3<float>& GetPos()const { return m_nowPos; }
	const Vec3<float>& GetTarget()const { return m_nowTarget; }
	float GetTimeRate() { return m_timer.GetTimeRate(); }

	/// <summary>
	/// Imgui�f�o�b�O�ō��ڒǉ�
	/// </summary>
	/// <param name="arg_tag">���[�V�����Ɋւ���p�����[�^�ݒ�̍ۂɃ��x���ɕt�^���閼�O</param>
	/// <param name="arg_changePos">�ω������������W�̕ύX��̒l�i�[��</param>
	/// <param name="arg_changeTarget">�ω������������W�̕ύX��̒l�i�[��</param>
	void ImguiDebugItems(
		std::string arg_tag, 
		const Vec3<float>* arg_changePos = nullptr, 
		const Vec3<float>* arg_changeTarget = nullptr);
};

