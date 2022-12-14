#pragma once
#include"KuroEngine.h"
#include"TimeScale.h"
#include"Timer.h"
#include"Light.h"
#include"Debugger.h"
#include"StartWave.h"
#include"ClearWave.h"
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
class WaveMgr;
class Screen;

class InGameScene : public BaseScene, public Debugger
{
	//プレイヤー
	std::shared_ptr<Player>m_player;

	//スロットマシン
	std::shared_ptr<SlotMachine>m_slotMachine;

	//ライトマネージャ
	std::shared_ptr<LightManager>m_ligMgr;
	Light::Direction m_dirLig;

	//背景画像
	std::shared_ptr<TextureBuffer>m_backGround;

	//ゲームカメラ
	const std::string m_backCamKey = "GameCamera - Back";
	const std::string m_frontCamKey = "GameCamera - Front";
	std::shared_ptr<GameCamera>m_gameCam;

	//タイムスケール
	TimeScale m_timeScale;

	//コリジョンマネージャ
	std::shared_ptr<CollisionManager>m_collisionMgr;

	//ウェーブマネージャ
	std::shared_ptr<WaveMgr>m_waveMgr;

	//ステージマネージャ
	std::shared_ptr<StageMgr>m_stageMgr;

	//オブジェクトマネージャ
	std::shared_ptr<ObjectManager>m_objMgr;

	//エネミーエミッター
	std::shared_ptr<EnemyEmitter>m_enemyEmitter;

	//大型スクリーン
	std::shared_ptr<Screen>m_screen;

	//ウェーブスタート時のイベント
	std::shared_ptr<StartWave>m_startWaveEvent;

	//ウェーブクリア時のイベント
	std::shared_ptr<ClearWave>m_clearWaveEvent;

	//コライダーのデバッグ描画フラグ
	bool m_isDrawCollider = false;
	//エッジの描画フラグ
	bool m_isDrawEdge = true;

	void OnImguiItems()override;

public:
	InGameScene();
	void OnInitialize()override;
	void OnUpdate()override;
	void OnDraw()override;
	void OnImguiDebug()override;
	void OnFinalize()override;
};