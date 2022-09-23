#pragma once
#include<vector>
#include<memory>
#include"Transform.h"
#include"Camera.h"
#include"KuroMath.h"
class ActPoint;
class MarkingSystem;

class RushSystem
{
private:
	//�J����
	static std::shared_ptr<Camera>s_cam;
public:
	//�J�����L�[
	static const std::string s_cameraKey;
	//�J�����|�C���^
	static std::shared_ptr<Camera> GetCam()
	{
		if (!s_cam)s_cam = std::make_shared<Camera>("RushCamera");
		return s_cam;
	}

private:
	//�����_�Ƃ̋���
	float m_camDistToTarget = 27.0f;
	//�J�����ƒ����_�̍����̍�
	float m_camHeightOffset = 12.0f;
	//�J�����̒����_�̍����I�t�Z�b�g
	float m_camTargetHeightOffset = 4.0f;
	//�����_�ւ̃x�N�g����]�p�x
	int m_camToTargetVecSpinDegreeMax = 45;

	//�}�[�L���O "����" �Ώۂ̃|�C���^�z��
	std::vector<std::weak_ptr<ActPoint>>m_markingPointArray;
	//���݂̃��b�V���Ώۂ̃C���f�b�N�X
	int m_rushTargetPointIdx = -1;

	//���Ԍv���p
	int m_timer;
	//���b�V���S�̂̎��Ԍv��
	int m_totalRushTimer;
	//���b�V���S�̂ɂ����鎞��
	int m_totalRushTime;

	//�P���b�V���ɂ����鎞��
	int m_oneRushTime;
	int m_oneRushTimeMin = 20;	//�����i�ō����j
	int m_oneRushTimeMax = 30;	//����i�Œᑬ�j

	//�P���b�V���ɂ����鎞�Ԃ̃}�[�L���O�Ώۂ̐��̕ψʊ�i�����j
	int m_markCountIndicatorMin = 0;		//���̃}�[�L���O�񐔈ȉ��ōŒᑬ
	int m_markCountIndicatorMax = 10;	//���̃}�[�L���O�񐔈ȏ�ōō���

	//�P���b�V���Η�
	int m_oneRushDamage = 0;

	//���b�V���Ő�����΂��^�C�~���O�i�P���b�V���̃^�C�}�[�������Z�j
	float m_blowTiming = 0.0f;

	//���b�V�����ɑΏۂɋ߂Â����Ƃ��̑ΏۂƂ̋���
	float m_distToTarget = 2.0f;

	//�����b�V�����o�J�����t���O
	bool m_strongRushFlg = false;
	//�����b�V���̕~��
	float m_strongRushThreshold = 0.0f;
	//�����b�V�����o�J�����̏��
	struct StrongRushCamInfo
	{
		//�J���������̒��S
		Vec3<float>m_center;
		//���S�����Y����]�p�x
		Angle m_ySpinAngle;
		//���S�����Y����]�p�x_�ő�I�t�Z�b�g
		Angle m_ySpinAngleOffset = Angle(35);
		//���W��]�ړ��̃��[�V�����̃C�[�W���O
		EASE_CHANGE_TYPE m_spinEaseChangeType = EASE_CHANGE_TYPE::InOut;
		EASING_TYPE m_spinEaseType = EASING_TYPE::Sine;

		//�J�����̈ʒu�Ƌ����̒��S�Ƃ̍����I�t�Z�b�g
		float m_heightOffset = 1.0f;

		//���S����̋���
		float m_distToCenter;
		//�J�����̋������
		Vec3<float>m_spaceMin;
		Vec3<float>m_spaceMax;

		//���S����̋���_�ŏ�
		Vec3<float> m_distToCenterOffsetMin = { 8.0f,4.0f,6.0f };
		//���S����̋����̒ǉ���
		float m_distToCenterAdditional = 9.0f;

		//�����������A�j���[�V�����ɂ����鎞�ԁi�O�`�P�j
		float m_distMotionRate = 0.5f;
		//����������郂�[�V�����̃C�[�W���O
		EASE_CHANGE_TYPE m_distEaseChangeType = EASE_CHANGE_TYPE::Out;
		EASING_TYPE m_distEaseType = EASING_TYPE::Cubic;

		//��ƂȂ�x�N�g��
		Vec3<float>m_criteriaVec;


		//������
		void Init(const std::vector<std::weak_ptr<ActPoint>>& TargetArray, Vec3<float>PlayerPos);
		//�X�V
		void Update(const float& Rate);
	}m_strongRushCamInfo;

	//�C�[�W���O�ړ��̊J�n���W
	Vec3<float>m_easingStartPos;
	//�C�[�W���O�ړ��̏I�����W
	Vec3<float>m_easingEndPos;

	//�������܂ꂽ�I�u�W�F�N�g�����������W
	Vec3<float>m_involvedObjsTargetPos;
	//�P���b�V���̂ӂ���΂���������
	bool m_blow;
	//�ӂ���΂��͍ő�
	float m_blowPowerMax = 17.0f;
	//�ӂ���΂��̐���
	float m_blowRate = 0.0f;

	//�ӂ���΂��ꂽ�Ƃ��̐���Y������
	float m_blowPowerY = 1.4f;
	//�ӂ���΂���̗������x�ɂ�����d�͂̓x����
	float m_blowGravityRate = 0.5f;

public:
	void Init();
	void Update(Transform& PlayerTransform, const float& Gravity);

	void Excute(MarkingSystem& MarkingSys, const Vec3<float>& PlayerPos, bool& IsOneShot, const float& OneShotChargeRate);
	bool IsActive()const { return !m_markingPointArray.empty(); }

	//imgui�f�o�b�O�@�\
	void ImguiDebug();

	//�Ō�̃��b�V���̍Œ���
	bool IsLastRush()const { return m_rushTargetPointIdx == 0; }
	//���b�V�����I�����Ă��邩
	bool IsFinishRush()const { return m_markingPointArray.empty(); }

	//�������܂ꂽ�I�u�W�F�N�g�����������W
	const Vec3<float>& GetInvolvedObjsTargetPos()const 
	{ 
		//�Ō�̃��b�V���̂Ƃ��̓m�b�N�o�b�N����̂ŕs��
		assert(!IsLastRush());
		return m_involvedObjsTargetPos; 
	}
	const float& GetBlowRate()const
	{
		//���b�V���ɂ��ӂ���΂��̎��Ԋ���
		return m_blowRate;
	}
	//���݂̃��b�V���̐�����΂���
	float GetNowBlowPower()const;
	//�ӂ���΂����̐���Y������
	const float& GetBlowPowerY()const
	{
		return m_blowPowerY;
	}
	//�ӂ���΂����̏d�͂̓x����
	const float& GetBlowGravityRate()const
	{
		return m_blowGravityRate;
	}

	//�P���b�V���̋�؂�̃^�C�~���O
	bool IsOneRushTrigger()const
	{
		return m_timer <= 0;
	}

	//�P���b�V���̉Η�
	const int& GetOneRushDamage()const
	{
		assert(IsActive());
		return m_oneRushDamage;
	}
};

