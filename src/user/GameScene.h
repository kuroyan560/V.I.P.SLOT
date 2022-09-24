#pragma once
#include"KuroEngine.h"
class Player;
class LightManager;
class ModelObject;

class GameScene : public BaseScene
{
	//プレイヤー
	std::shared_ptr<Player>m_player;

	//ライトマネージャ
	std::shared_ptr<LightManager>m_ligMgr;

	//床
	std::shared_ptr<ModelObject>m_floorObj;

	//スロットマシン
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//コイン投入口
	std::shared_ptr<ModelObject>m_coinPortObj;

public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};