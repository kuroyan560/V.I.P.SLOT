#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"Angle.h"
class ModelObject;
class LightManager;
class Camera;

class Player
{
	//モデルオブジェクト
	std::shared_ptr<ModelObject>m_modelObj;

	//初期位置
	Vec3<float>m_startPos = { 0,0,-20 };

	//座標角度（０でスロット正面）
	Angle m_posAngle;

	//座標角移動量
	Angle m_moveAngle;

	//落下速度
	float m_fallSpeed;
	//落下加速度
	float m_fallAccel;

	//接地フラグ
	bool m_isOnGround;

public:
	Player();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};