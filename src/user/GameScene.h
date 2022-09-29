#pragma once
#include"KuroEngine.h"
#include"TimeScale.h"
class Player;
class SlotMachine;
class LightManager;
class ModelObject;
class TextureBuffer;
class GameCamera;
class EnemyManager;

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

	//敵マネージャ
	std::shared_ptr<EnemyManager>m_enemyMgr;
	
	//ゲームカメラ
	const std::string m_backCamKey = "GameCamera - Back";
	const std::string m_frontCamKey = "GameCamera - Front";
	std::shared_ptr<GameCamera>m_gameCam;

	//タイムスケール
	TimeScale m_timeScale;

public:
	GameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};