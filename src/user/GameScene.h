#pragma once
#include"KuroEngine.h"
class Player;
class LightManager;
class ModelObject;

class GameScene : public BaseScene
{
	//�v���C���[
	std::shared_ptr<Player>m_player;

	//���C�g�}�l�[�W��
	std::shared_ptr<LightManager>m_ligMgr;

	//��
	std::shared_ptr<ModelObject>m_floorObj;

	//�X���b�g�}�V��
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//�R�C��������
	std::shared_ptr<ModelObject>m_coinPortObj;

public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};