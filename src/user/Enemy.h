#pragma once
#include"CoinVault.h"
#include<memory>
#include"Transform.h"
class EnemyBreed;
class EnemyController;
class LightManager;
class Camera;

class Enemy
{
protected:
	Enemy() {}

	friend class EnemyController;

	//血統
	std::weak_ptr<EnemyBreed>m_breed;
	//トランスフォーム
	Transform m_transform;
	//所持金
	CoinVault m_coinVault;
	//HP
	int m_hp;
	//コントローラー
	std::unique_ptr<EnemyController>m_controller;

public:
	//初期化
	void Init(const std::shared_ptr<EnemyBreed>& arg_breed);
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//種別番号ゲッタ
	const int& GetTypeID();

	//生存フラグ
	bool IsDead();
};