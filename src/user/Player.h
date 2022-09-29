#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
#include"Timer.h"
class ModelObject;
class LightManager;
class Camera;
class SlotMachine;
class TimeScale;

class Player
{
	//モデルオブジェクト
	std::shared_ptr<ModelObject>m_modelObj;

	//移動
	Vec3<float>m_move = { 0,0,0 };

	//落下速度
	float m_fallSpeed;

	//接地フラグ
	bool m_isOnGround;

	//所持金
	CoinVault m_coinVault;

	//BETのスパン計測用タイマー
	Timer m_betTimer;
	//連続BETの計測用タイマー
	Timer m_consecutiveBetTimer;
	//BETのSE
	int m_betSE;

public:
	Player();
	//初期化
	void Init();
	//更新
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, const TimeScale& arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	CoinVault& GetVault() { return m_coinVault; }
};