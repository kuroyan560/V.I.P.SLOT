#pragma once
#include<memory>
#include<array>
#include<string>
#include"Reel.h"
class ModelObject;
class LightManager;
class Camera;

class SlotMachine
{
	//スロットマシン
	std::shared_ptr<ModelObject>m_slotMachineObj;

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
	int m_slotWaitTimer;

	//サウンド
	int m_spinStartSE;	//回転スタート
	int m_reelStopSE;		//リールストップ
	
public:
	SlotMachine();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};