#pragma once
#include<memory>
class Enemy;
class TimeScale;
class LightManager;
class Camera;

class EnemyController
{
	friend class Enemy;

	virtual void OnInit(Enemy& arg_enemy) = 0;
	virtual void OnUpdate(Enemy& arg_enemy, const TimeScale& arg_timeScale) = 0;

	//�f�t�H���g�ł͂������f����`�悷�邾��
	virtual void OnDraw(Enemy& arg_enemy, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);

	//�_���[�W���󂯂��Ƃ��̉��o�Ȃ�
	virtual void OnDamage(Enemy& arg_enemy) {};

	//�S�Ă̓������I�����Ƃ��ȂǁAEnemyController�����玀�S�t���O�𗧂Ă���悤��
	virtual bool IsDead(Enemy& arg_enemy) { return false; }

	//�N���[���̐���
	virtual std::unique_ptr<EnemyController>Clone() = 0;
};

//���ړ�
class EnemySlideMove : public EnemyController
{
	//�X�s�[�h
	float m_xSpeed;

	//X�������X�s�[�h
	float m_xMove;

	void OnInit(Enemy& arg_enemy)override;
	void OnUpdate(Enemy& arg_enemy, const TimeScale& arg_timeScale)override;
	std::unique_ptr<EnemyController>Clone()override;
	bool IsDead(Enemy& arg_enemy)override;

public:
	EnemySlideMove(float arg_xSpeed) :m_xSpeed(arg_xSpeed) {}
};