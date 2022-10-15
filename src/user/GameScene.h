#pragma once
#include"KuroEngine.h"
#include"TimeScale.h"
#include"Timer.h"
class Player;
class SlotMachine;
class LightManager;
class ModelObject;
class TextureBuffer;
class GameCamera;
class ObjectManager;
class CollisionManager;
class StageMgr;

class GameScene : public BaseScene
{
	//�v���C���[
	std::shared_ptr<Player>m_player;

	//�X���b�g�}�V��
	std::shared_ptr<SlotMachine>m_slotMachine;

	//���C�g�}�l�[�W��
	std::shared_ptr<LightManager>m_ligMgr;

	//��
	std::shared_ptr<ModelObject>m_squareFloorObj;

	//�w�i�摜
	std::shared_ptr<TextureBuffer>m_backGround;

	//�Q�[���J����
	const std::string m_backCamKey = "GameCamera - Back";
	const std::string m_frontCamKey = "GameCamera - Front";
	std::shared_ptr<GameCamera>m_gameCam;

	//�^�C���X�P�[��
	TimeScale m_timeScale;

	//�R���W�����}�l�[�W��
	std::shared_ptr<CollisionManager>m_collisionMgr;

	//�X�e�[�W�}�l�[�W��
	std::shared_ptr<StageMgr>m_stageMgr;

public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};