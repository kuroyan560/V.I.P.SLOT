#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
#include"Angle.h"
#include"KuroMath.h"
class GameObject;
class ObjectManager;
class CollisionManager;
class TimeScale;
class LightManager;
class Camera;

class ObjectController
{
	friend class GameObject;

protected:
	//�I�u�W�F�N�g�}�l�[�W��
	static std::weak_ptr<ObjectManager>s_objMgr;

public:
	static void AttachObjectManager(std::weak_ptr<ObjectManager>arg_objMgr)
	{
		s_objMgr = arg_objMgr;
	}

protected:
	//�������z�֐��i�A�����W�K�{�j
	//������
	virtual void OnInit(GameObject& arg_obj) = 0;
	//�X�V
	virtual void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr) = 0;
	//�N���[���̐���
	virtual std::unique_ptr<ObjectController>Clone() = 0;

	//���z�֐��i�A�����W�\�j
	//�f�t�H���g�ł͂������f����`�悷�邾��
	virtual void OnDraw(GameObject& arg_obj, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);
	//�_���[�W���󂯂��Ƃ��̉��o�Ȃǁi�Ԃ�l�F�R�C�����j
	virtual int OnDamage(GameObject& arg_obj, int arg_damageAmount) { return 0; }
	//�ޏꂵ����
	virtual bool IsLeave(GameObject& arg_obj)const { return false; }

	//���̑��i�֐��Œ�j
	//�A�^�b�`����Ă���I�u�W�F�N�g��HP���O�ȉ���
	bool IsObjsHpZero(GameObject& arg_obj)const;
	//�S�Ă̓������I�����Ƃ��ȂǁAEnemyController�����玀�S�t���O�𗧂Ă���悤��
	bool IsDead(GameObject& arg_obj)const { return IsLeave(arg_obj) || IsObjsHpZero(arg_obj); }
	//��ʊO�ɏo����
	bool IsOutOfScreen(GameObject& arg_obj)const;
};

//�w�肵�������Ɉړ�
class OC_DirectionMove : public ObjectController
{
	//�X�^�[�g�ʒu
	Vec3<float>m_startPos;

	//XY���ʈړ�����
	Vec2<float>m_moveDirXY;

	//�X�s�[�h
	float m_speed = 1.0f;

	//�֍s����ۂ̃��W�A��
	Angle m_radian;
	//�֍s���邩
	bool m_sinMeandeling = false;
	//�֍s�̃C���^�[�o��
	float m_meandelingInterval = 60.0f;
	//�֍s�̊p�x
	Angle m_meandelingAngle = Angle(30);

	void OnInit(GameObject& arg_obj)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override { return IsOutOfScreen(arg_obj); }

public:
	/// <summary>
	/// �p�����[�^�ݒ�
	/// </summary>
	/// <param name="arg_startPos">�����ʒu</param>
	void SetParameters(Vec3<float>arg_startPos) { m_startPos = arg_startPos; }

	/// <summary>
	/// �p�����[�^�ݒ�
	/// </summary>
	/// <param name="arg_startPos">�����ʒu</param>
	/// <param name="arg_moveDirXY">XY���ʈړ�����</param>
	/// <param name="arg_speed">�ړ��X�s�[�h</param>
	/// <param name="arg_sinMeandeling">�֍s���邩�isin�J�[�u�j</param>
	/// <param name="arg_meandelingInterval">�֍s�̃C���^�[�o��</param>
	/// <param name="arg_meandelingAngle">�֍s�̊p�x</param>
	void SetParameters(
		Vec3<float>arg_startPos,
		Vec2<float>arg_moveDirXY,
		float arg_speed,
		bool arg_sinMeandeling = false,
		float arg_meandelingInterval = 60.0f,
		Angle arg_meandelingAngle = Angle(30))
	{
		m_startPos = arg_startPos;
		m_moveDirXY = arg_moveDirXY;
		m_speed = arg_speed;
		m_sinMeandeling = arg_sinMeandeling;
		m_meandelingInterval = arg_meandelingInterval;
		m_meandelingAngle = arg_meandelingAngle;
	}
};

//�w�肵���ʒu�ɃC�[�W���O�֐���p���Ĉړ�
class OC_DestinationEaseMove : public ObjectController
{
	//�C�[�W���O���
	EASE_CHANGE_TYPE m_easeChangeType;
	EASING_TYPE m_easeType;
	//�X�^�[�g�n�_
	Vec3<float>m_startPos;
	//�ړI�n
	Vec3<float>m_destinationPos;
	//����
	float m_interval;
	//�^�C�}�[
	Timer m_timer;

	void OnInit(GameObject& arg_obj)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override { return m_timer.IsTimeUp(); }

public:
	OC_DestinationEaseMove(EASE_CHANGE_TYPE arg_easeChangeType,EASING_TYPE arg_easeType, float arg_interval)
		:m_easeChangeType(arg_easeChangeType),m_easeType(arg_easeType), m_interval(arg_interval) {}

	/// <summary>
	/// �p�����[�^�ݒ�
	/// </summary>
	/// <param name="arg_startPos">�X�^�[�g�n�_</param>
	/// <param name="arg_destinationPos">�ړI�n</param>
	void SetParameters(Vec3<float>arg_startPos, Vec3<float>arg_destinationPos)
	{
		m_startPos = arg_startPos;
		m_destinationPos = arg_destinationPos;
	}
};

//��ђ��ˁ��e�𔭎ˁi�X���C���C��A��ʊO����o����ђ��ˁ��V���b�g�ňړ��j
class OC_SlimeBattery : public ObjectController
{
public:
	//�X�e�[�^�X�Ǘ�
	enum STATUS { APPEAR, WAIT, JUMP_AND_SHOT, DISAPPEAR, NUM };

private:
	STATUS m_status;
	//���Ԏw��
	std::array<float, STATUS::NUM>m_intervalTimes;

	//���Ԍv��
	Timer m_timer;

	//�ړ���Y
	float m_moveY;

	//�o�ꎞ��Y
	float m_appearY = 3.0f;

	//�W�����v��
	float m_jumpPower = 2.0f;

	//�o���n��X�����W
	float m_departureX = 0.0f;

	//�ړI�n��X�����W
	float m_destinationX = 0.0f;

	//��ђ��ˈʒu
	std::vector<float>m_spotXList;
	//���݂̍��W�X�C���f�b�N�X
	int m_spotXIdx = 0;

	void OnInit(GameObject& arg_obj)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override;

	//�߂��[��X��Ԃ�
	float GetNearEdgePosX(float arg_posX)const;

public:
	OC_SlimeBattery(float* arg_intervalTimes, float arg_jumpPower) :m_jumpPower(arg_jumpPower)
	{
		for (int i = 0; i < STATUS::NUM; ++i)m_intervalTimes[i] = arg_intervalTimes[i];
	}

	/// <summary>
	/// �p�����[�^�ݒ�
	/// </summary>
	/// <param name="arg_appearY">�o�ꎞ��Y���W</param>
	/// <param name="arg_destinationXArray">�ڕW�n�_X���W�z��</param>
	/// <param name="arg_arraySize">�z��̗v�f��</param>
	void SetParameters(float arg_appearY, float arg_destinationXArray[], size_t arg_arraySize)
	{
		m_spotXList.clear();
		for (int i = 0; i < static_cast<int>(arg_arraySize); ++i)
		{
			m_spotXList.emplace_back(arg_destinationXArray[i]);
		}
		m_appearY = arg_appearY;
	}
};