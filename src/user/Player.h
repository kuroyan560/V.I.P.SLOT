#pragma once
#include<string>
#include<memory>
class ModelObject;
class LightManager;
class Camera;

class Player
{
	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject>m_modelObj;
public:
	Player();
	//������
	void Init();
	//�X�V
	void Update();
	//�`��
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};