#pragma once
#include"KuroEngine.h"
class Player;
class LightManager;

class GameScene : public BaseScene
{
	//�v���C���[
	std::shared_ptr<Player>m_player;

	//���C�g�}�l�[�W��
	std::shared_ptr<LightManager>m_ligMgr;

public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};