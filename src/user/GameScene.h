#pragma once
#include"KuroEngine.h"
class Player;
class LightManager;
class ModelObject;
class TextureBuffer;
class GameCamera;

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

	//�w�i�摜
	std::shared_ptr<TextureBuffer>m_backGround;
	
	//�Q�[���J����
	const std::string m_gameCamKey = "GameCamera";
	std::shared_ptr<GameCamera>m_gameCam;

public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};