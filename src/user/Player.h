#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"Angle.h"
class ModelObject;
class LightManager;
class Camera;

class Player
{
	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject>m_modelObj;

	//�����ʒu
	Vec3<float>m_startPos = { 0,0,-20 };

	//���W�p�x�i�O�ŃX���b�g���ʁj
	Angle m_posAngle;

	//���W�p�ړ���
	Angle m_moveAngle;

	//�������x
	float m_fallSpeed;
	//���������x
	float m_fallAccel;

	//�ڒn�t���O
	bool m_isOnGround;

public:
	Player();
	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};