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
#include"ConstParameters.h"
#include"Debugger.h"
class ModelObject;
class LightManager;
class GameCamera;
class TimeScale;
class Player;
class RenderTarget;

class SlotMachine : public Debugger
{
public:
	enum REEL { LEFT, CENTER, RIGHT, NUM, NONE = -1 };

private:
	//スロットマシン
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//コイン投入口メガホン
	std::shared_ptr<ModelObject>m_megaPhoneObj;

	//リール
	const std::array<std::string, REEL::NUM>REEL_MATERIAL_NAME =
	{
		"Reel_Left","Reel_Center","Reel_Right"
	};
	std::array<Reel, REEL::NUM>m_reels;

	//スロットのレバーフラグ（StartかStopか）
	int m_lever;

	//サウンド
	int m_spinStartSE;	//回転スタート
	int m_reelStopSE;		//リールストップ

	//絵柄の定義クラス
	PatternManager m_patternMgr;

	/*--- スロットゲージ ---*/
	//スロットゲージを映す画面レンダーターゲット
	std::shared_ptr<RenderTarget>m_slotGaugeScreen;
	//スロットゲージ画像
	std::shared_ptr<TextureBuffer>m_slotGaugeTex;
	//スロット回転予約
	int m_startSlotCount = 0;

	/*--- 自動操作 ---*/
	//自動操作タイマー
	std::array<Timer, ConstParameter::Slot::AUTO_OPERATE_NUM> m_autoTimer;
	//スロットゲージ自動操作タイムスケール（ゲージ量が多いほど時間間隔が早まる）
	float m_autoOperateTimeScale = 1.0f;

	//絵柄を確認して全て一緒なら効果発動
	bool CheckReelPattern();
	//スロットの結果から演出を選ぶ
	void SlotPerform(const ConstParameter::Slot::PATTERN& arg_pattern);
	
	//全リールが停止中か
	bool IsStop()const { return m_lever == REEL::NONE; }

	//スロットゲージ画面更新
	void UpdateSlotGaugeScreen();

	void OnImguiItems()override;

public:
	SlotMachine();
	//初期化
	void Init(	);
	//更新
	void Update(std::weak_ptr<Player>arg_player, const TimeScale& arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//レバー操作（絵柄発動でtrueを返す）
	bool Lever();

	//スロット予約
	void Booking();

	//リール設定
	void ReelSet(REEL arg_which, std::shared_ptr<TextureBuffer>& arg_reelTex, std::vector<ConstParameter::Slot::PATTERN>& arg_patternArray);
};