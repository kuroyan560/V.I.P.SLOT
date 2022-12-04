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
	virtual void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos) = 0;
	//�X�V
	virtual void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr) = 0;
	//�N���[���̐���
	virtual std::unique_ptr<ObjectController>Clone() = 0;

	//���z�֐��i�A�����W�\�j
	//�f�t�H���g�ł͂������f����`�悷�邾��
	virtual void OnDraw(GameObject& arg_obj, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);
	//�_���[�W���󂯂��Ƃ��̉��o�Ȃ�
	virtual void OnDamage(GameObject& arg_obj, int arg_damageAmount) {}
	//�ޏꂵ����
	virtual bool IsLeave(GameObject& arg_obj)const { return false; }

	//���̑��i�֐��Œ�j
	//�A�^�b�`����Ă���I�u�W�F�N�g��HP���O�ȉ���
	bool IsObjsHpZero(GameObject& arg_obj)const;
	//�S�Ă̓������I�����Ƃ��ȂǁAEnemyController�����玀�S�t���O�𗧂Ă���悤��
	bool IsDead(GameObject& arg_obj)const { return IsLeave(arg_obj) || IsObjsHpZero(arg_obj); }
	//��ʊO�ɏo����
	bool IsOutOfScreen(GameObject& arg_obj)const;
	//�ΏۃI�u�W�F�N�g�̃��[�J���^�C���X�P�[��
	float GetLocalTimeScale(GameObject& arg_obj)const;
};

//�w�肵�������Ɉړ�
class OC_DirectionMove : public ObjectController
{
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

	void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override { return IsOutOfScreen(arg_obj); }

public:
	/// <summary>
	/// �p�����[�^�ݒ�
	/// </summary>
	/// <param name="arg_moveDirXY">XY���ʈړ�����</param>
	/// <param name="arg_speed">�ړ��X�s�[�h</param>
	/// <param name="arg_sinMeandeling">�֍s���邩�isin�J�[�u�j</param>
	/// <param name="arg_meandelingInterval">�֍s�̃C���^�[�o��</param>
	/// <param name="arg_meandelingAngle">�֍s�̊p�x</param>
	void SetParameters(
		Vec2<float>arg_moveDirXY,
		float arg_speed,
		bool arg_sinMeandeling = false,
		float arg_meandelingInterval = 60.0f,
		Angle arg_meandelingAngle = Angle(30))
	{
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
protected:
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
	//�S�[�����Ă��邩
	bool m_isGoal = false;

	void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override;
	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override { return m_isGoal; }

public:
	OC_DestinationEaseMove(EASE_CHANGE_TYPE arg_easeChangeType,EASING_TYPE arg_easeType, float arg_interval)
		:m_easeChangeType(arg_easeChangeType),m_easeType(arg_easeType), m_interval(arg_interval) {}

	/// <summary>
	/// �p�����[�^�ݒ�
	/// </summary>
	/// <param name="arg_destinationPos">�ړI�n</param>
	void SetParameters(Vec3<float>arg_destinationPos)
	{
		m_destinationPos = arg_destinationPos;
	}
};

//�w�肵���Ώ�(GameObject)�Ɍ������ăC�[�W���O�֐���p���Ĉړ�
class OC_TargetObjectEaseMove : public OC_DestinationEaseMove
{
	//�ΏۂƂȂ�Q�[���I�u�W�F�N�g
	GameObject* m_target;

	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	std::unique_ptr<ObjectController>Clone()override;

public:
	OC_TargetObjectEaseMove(EASE_CHANGE_TYPE arg_easeChangeType, EASING_TYPE arg_easeType, float arg_interval)
		:OC_DestinationEaseMove(arg_easeChangeType, arg_easeType, arg_interval) {}

	/// <summary>
	/// �p�����[�^�ݒ�
	/// </summary>
	/// <param name="arg_destinationPos">�ړI�n</param>
	void SetParameters(GameObject* arg_target);
};

//��ђ��ˁ��e�𔭎ˁi�X���C���C��A��ʊO����o����ђ��ˁ��V���b�g�ňړ��j
class OC_SlimeBattery : public ObjectController
{
public:
	//�X�e�[�^�X�Ǘ�
	enum STATUS { WAIT, JUMP_AND_SHOT, NUM };

private:
	STATUS m_status;

	//���Ԍv��
	Timer m_timer;

	//�ړ���Y
	float m_moveY;

	//�o���n��X�����W
	float m_departureX = 0.0f;

	//�ړI�n��X�����W
	float m_destinationX = 0.0f;

	//�ޏ�t���O
	bool m_isLeave = false;

	void OnUpdate(GameObject& arg_obj, const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr)override;
	bool IsLeave(GameObject& arg_obj)const override
	{
		return m_isLeave;
	}

protected:
	OC_SlimeBattery() {}

	virtual std::unique_ptr<ObjectController>Clone() = 0;
	virtual void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override;
	//��ђ��ˎ��ɌĂяo������
	virtual void OnSlimeJump() = 0;
	/// <summary>
	/// ���̖ړI�nX����
	/// </summary>
	/// <param name="arg_obj">�A�^�b�`����Ă���I�u�W�F�N�g</param>
	/// <param name="arg_decideNextX">�ړI�nX���i�[���邽�߂̕ϐ�</param>
	/// <returns>�ړI�n�����邩</returns>
	virtual bool OnDecideNextDetinationX(GameObject& arg_obj, float& arg_decideNextX) = 0;

public:
	virtual ~OC_SlimeBattery() {}
};

//��ђ��ˁ��e�𔭎ˁi�X���C���C��A��ʊO����o����ђ��ˁ��V���b�g�ňړ��j�����̏�ŁBX�ړ����Ȃ�
class OC_SlimeBattery_FixedX : public OC_SlimeBattery
{
	int m_countMax = 3;
	int m_count;

	std::unique_ptr<ObjectController>Clone()override
	{
		return std::make_unique<OC_SlimeBattery_FixedX>();
	}
	void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override
	{
		OC_SlimeBattery::OnInit(arg_obj, arg_initPos);
		m_count = m_countMax;
	}
	void OnSlimeJump()override {}
	bool OnDecideNextDetinationX(GameObject& arg_obj, float& arg_decideNextX)override;

public:
	//��ђ��ˉ�
	void SetJumpCountMax(int arg_countMax) { m_countMax = arg_countMax; }
};


//��ђ��ˁ��e�𔭎ˁi�X���C���C��A��ʊO����o����ђ��ˁ��V���b�g�ňړ��j�����[�g�w��
class OC_SlimeBattery_RouteDefined : public OC_SlimeBattery
{
	//��ђ��ˈʒu
	std::vector<float>m_spotXList;
	//���݂̍��W�X�C���f�b�N�X
	int m_spotXIdx = 0;

	std::unique_ptr<ObjectController>Clone()override
	{
		return std::make_unique<OC_SlimeBattery_RouteDefined>();
	}
	void OnInit(GameObject& arg_obj, Vec3<float>arg_initPos)override;
	void OnSlimeJump()override
	{
		m_spotXIdx++;
	}
	bool OnDecideNextDetinationX(GameObject& arg_obj, float& arg_decideNextX)override
	{
		//���̖ړI�n�����݂��邩
		bool existNext = m_spotXIdx < static_cast<int>(m_spotXList.size());
		if (existNext)
		{
			//�C���f�b�N�X���C���N�������g���Ď��̖ړI�nX�w��
			arg_decideNextX = m_spotXList[m_spotXIdx];
		}
		return existNext;
	}

public:
	/// <summary>
	/// X���������[�g�w��
	/// </summary>
	/// <param name="arg_destinationXArray">�ڕW�n�_X���W�z��</param>
	/// <param name="arg_arraySize">�z��̗v�f��</param>
	void SetRouteX(float arg_destinationXArray[], size_t arg_arraySize)
	{
		m_spotXList.clear();
		for (int i = 0; i < static_cast<int>(arg_arraySize); ++i)
		{
			m_spotXList.emplace_back(arg_destinationXArray[i]);
		}
	}
};

//��ђ��ˁ��e�𔭎ˁi�X���C���C��A��ʊO����o����ђ��ˁ��V���b�g�ňړ��j���v���C���[��ǂ�
class OC_SlimeBattery_ChasePlayer : public OC_SlimeBattery
{

};
