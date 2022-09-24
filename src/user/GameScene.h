#pragma once
#include"KuroEngine.h"
class Player;
class LightManager;

class GameScene : public BaseScene
{
	//プレイヤー
	std::shared_ptr<Player>m_player;

	//ライトマネージャ
	std::shared_ptr<LightManager>m_ligMgr;

public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};