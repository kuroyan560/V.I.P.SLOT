#pragma once
#include"KuroEngine.h"
#include"TimeScale.h"
#include"Timer.h"
#include"Light.h"
class Player;
class SlotMachine;
class LightManager;
class ModelObject;
class TextureBuffer;
class GameCamera;
class ObjectManager;
class CollisionManager;
class StageMgr;
class EnemyEmitter;

class InGameScene : public BaseScene
{
	SceneTransition m_sceneTrans;

	//�v���C���[
	std::shared_ptr<Player>m_player;

	//�X���b�g�}�V��
	std::shared_ptr<SlotMachine>m_slotMachine;

	//���C�g�}�l�[�W��
	std::shared_ptr<LightManager>m_ligMgr;
	Light::Direction m_dirLig;

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

	//�I�u�W�F�N�g�}�l�[�W��
	std::shared_ptr<ObjectManager>m_objMgr;

	//�G�l�~�[�G�~�b�^�[
	std::shared_ptr<EnemyEmitter>m_enemyEmitter;

	//�R���C�_�[�̃f�o�b�O�`��t���O
	bool m_isDrawCollider = false;

public:
	InGameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};