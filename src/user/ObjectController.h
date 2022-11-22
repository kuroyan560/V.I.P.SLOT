#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
class GameObject;
class TimeScale;
class LightManager;
class Camera;

class ObjectController
{
	friend class GameObject;

protected:
	//�������z�֐��i�A�����W�K�{�j
	//������
	virtual void OnInit(GameObject& arg_enemy) = 0;
	//�X�V
	virtual void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale) = 0;
	//�N���[���̐���
	virtual std::unique_ptr<ObjectController>Clone() = 0;

	//���z�֐��i�A�����W�\�j
	//�f�t�H���g�ł͂������f����`�悷�邾��
	virtual void OnDraw(GameObject& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);
	//�_���[�W���󂯂��Ƃ��̉��o�Ȃǁi�Ԃ�l�F�R�C�����j
	virtual int OnDamage(GameObject& arg_enemy, int arg_damageAmount) { return 0; }
	//�ޏꂵ����
	virtual bool IsLeave(GameObject& arg_obj)const { return false; }

	//���̑��i�֐��Œ�j
	//�A�^�b�`����Ă���I�u�W�F�N�g��HP���O�ȉ���
	bool IsObjsHpZero(GameObject& arg_obj)const;
	//�S�Ă̓������I�����Ƃ��ȂǁAEnemyController�����玀�S�t���O�𗧂Ă���悤��
	bool IsDead(GameObject& arg_obj) { return IsLeave(arg_obj) || IsObjsHpZero(arg_obj); }


};

//���ړ�
class OC_SlideMove : public ObjectController
{
	//�X�s�[�h
	float m_xSpeed;

	//X�������X�s�[�h
	float m_xMove;

	//���WY
	float m_posY;

	void OnInit(GameObject& arg_enemy)override;
	void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override;

public:
	OC_SlideMove(float arg_xSpeed)
	{
		m_xSpeed = arg_xSpeed;
	}
	void SetPosY(float arg_posY) { m_posY = arg_posY; }
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

	void OnInit(GameObject& arg_enemy)override;
	void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsLeave(GameObject& arg_obj)const override;

	//�߂��[��X��Ԃ�
	float GetNearEdgePosX(float arg_posX)const;

public:
	OC_SlimeBattery(float* arg_intervalTimes, float arg_jumpPower) :m_jumpPower(arg_jumpPower)
	{
		for (int i = 0; i < STATUS::NUM; ++i)m_intervalTimes[i] = arg_intervalTimes[i];
	}

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