#pragma once
#include<string>
#include<memory>
#include"Vec.h"
class ModelObject;
class LightManager;
class Camera;

class Player
{
	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject>m_modelObj;

	//�X�^�[�g�ʒu
	Vec3<float>m_startPos = { 0,0,-20 };

	//�ړ�
	Vec3<float>m_move = { 0,0,0 };

	//�������x
	float m_fallSpeed;

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