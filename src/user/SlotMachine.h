#pragma once
#include<memory>
#include<array>
#include<string>
#include"Reel.h"
#include"CoinVault.h"
#include"Transform.h"
#include"Timer.h"
#include"PatternManager.h"
#include"CoinObjectManager.h"
#include"CoinPerform.h"
#include"ReturnCoinEmitter.h"
class ModelObject;
class LightManager;
class GameCamera;
class TimeScale;
class Player;

class SlotMachine
{
	//スロットマシン
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//コイン投入口メガホン
	std::shared_ptr<ModelObject>m_megaPhoneObj;

	//リール
	enum REEL { LEFT, CENTER, RIGHT, NUM };
	const std::array<std::string, REEL::NUM>REEL_MATERIAL_NAME =
	{
		"Reel_Left","Reel_Center","Reel_Right"
	};
	std::array<Reel, REEL::NUM>m_reels;

	//スロットのレバーフラグ（StartかStopか）
	int m_lever;
	//最後のリールを止めてからの時間計測
	Timer m_slotWaitTimer;

	//サウンド
	int m_spinStartSE;	//回転スタート
	int m_reelStopSE;		//リールストップ

	//絵柄の定義クラス
	PatternManager m_patternMgr;

	//絵柄を確認して全て一緒なら効果発動
	bool CheckReelPattern();
	//スロットの結果から演出を選ぶ
	void SlotPerform(const ConstParameter::Slot::PATTERN& arg_pattern);
	
public:
	SlotMachine();
	//初期化
	void Init();
	//更新
	void Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<GameCamera>arg_gameCam);

	//レバー操作
	void Lever();
};