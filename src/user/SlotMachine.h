#pragma once
#include<memory>
#include<array>
#include<string>
#include"Reel.h"
#include"CoinVault.h"
#include"Transform.h"
#include<forward_list>
class ModelObject;
class Model;
class LightManager;
class Camera;

class SlotMachine
{
	//スロットマシン
	std::shared_ptr<ModelObject>m_slotMachineObj;

	//コイン投入口メガホン
	std::shared_ptr<ModelObject>m_megaPhoneObj;

	//コインモデル
	std::shared_ptr<Model>m_coinModel;

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

	//所持金
	CoinVault m_coinVault;

	//BETされたコインの情報
	struct BetCoin
	{
		//相手の所持金
		CoinVault* m_otherVault;
		//コイン数
		int m_coinNum;
		//投げ込み位置
		Transform m_emitTransform;
		//自身のトランスフォーム
		Transform m_transform;
		//投げ込まれてからの時間
		int m_timer = 0;
		//BETに成功
		bool m_bet = false;

		BetCoin(CoinVault* arg_otherVault, int arg_coinNum, const Transform& arg_emitTransform)
			:m_otherVault(arg_otherVault), m_coinNum(arg_coinNum), m_emitTransform(arg_emitTransform), m_transform(arg_emitTransform) {}
	};
	std::forward_list<BetCoin>m_betCoinArray;
	
public:
	SlotMachine();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//BET受付
	void Bet(CoinVault& arg_otherVault, int arg_coinNum, const Transform& arg_emitTransform);
};