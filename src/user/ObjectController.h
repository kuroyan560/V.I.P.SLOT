#pragma once
#include<memory>
class GameObject;
class TimeScale;
class LightManager;
class Camera;

class ObjectController
{
	friend class GameObject;

	virtual void OnInit(GameObject& arg_enemy) = 0;
	virtual void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale) = 0;

	//�f�t�H���g�ł͂������f����`�悷�邾��
	virtual void OnDraw(GameObject& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);

	//�_���[�W���󂯂��Ƃ��̉��o�Ȃ�
	virtual void OnDamage(GameObject& arg_enemy) {};

	//�S�Ă̓������I�����Ƃ��ȂǁAEnemyController�����玀�S�t���O�𗧂Ă���悤��
	virtual bool IsDead(GameObject& arg_enemy) { return false; }

	//�N���[���̐���
	virtual std::unique_ptr<ObjectController>Clone() = 0;
};

//���ړ�
class ObjectSlideMove : public ObjectController
{
	//�X�s�[�h
	float m_xSpeed;

	//X�������X�s�[�h
	float m_xMove;

	void OnInit(GameObject& arg_enemy)override;
	void OnUpdate(GameObject& arg_enemy, const TimeScale& arg_timeScale)override;
	std::unique_ptr<ObjectController>Clone()override;
	bool IsDead(GameObject& arg_enemy)override;

public:
	ObjectSlideMove(float arg_xSpeed) :m_xSpeed(arg_xSpeed) {}
};