#pragma once
#include"KuroEngine.h"
class Player;
class SlotMachine;
class LightManager;
class ModelObject;
class TextureBuffer;
class GameCamera;

class GameScene : public BaseScene
{
	//プレイヤー
	std::shared_ptr<Player>m_player;

	//スロットマシン
	std::shared_ptr<SlotMachine>m_slotMachine;

	//ライトマネージャ
	std::shared_ptr<LightManager>m_ligMgr;

	//床
	std::shared_ptr<ModelObject>m_squareFloorObj;

	//背景画像
	std::shared_ptr<TextureBuffer>m_backGround;
	
	//ゲームカメラ
	const std::string m_backCamKey = "GameCamera - Back";
	const std::string m_frontCamKey = "GameCamera - Front";
	std::shared_ptr<GameCamera>m_gameCam;

public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};