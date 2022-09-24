#pragma once
#include<string>
#include<memory>
#include"Vec.h"
class ModelObject;
class LightManager;
class Camera;

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

public:
	//スタート位置
	const Vec3<float>m_startPos = { 0,0,-20 };

	Player();
	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam);
};