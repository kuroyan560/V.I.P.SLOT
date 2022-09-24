#pragma once
#include"KuroEngine.h"
class Player;
class LightManager;
class ModelObject;
class TextureBuffer;
class GameCamera;

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

	//背景画像
	std::shared_ptr<TextureBuffer>m_backGround;
	
	//ゲームカメラ
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