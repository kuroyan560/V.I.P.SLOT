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
private:
	//所持金
	CoinVault m_coinVault;
	//HP
	int m_hp;
	//コントローラー
	std::unique_ptr<EnemyController>m_controller;

public:
	//トランスフォーム
	Transform m_transform;
	//血統
	std::weak_ptr<EnemyBreed>m_breed;

	//血統情報を基に生成
	Enemy(const std::shared_ptr<EnemyBreed>& arg_breed);

	//初期化
	void Init();
	//更新
	void Update();
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	/// <summary>
	/// ダメージを受ける
	/// </summary>
	/// <param name="arg_amount">ダメージ量</param>
	/// <returns>死亡した場合コイン数、死亡してなければ０を返す/returns>
	int Damage(int arg_amount = 1);
	//一撃で死ぬ
	int Blow() { return Damage(m_hp); }

	//種別番号ゲッタ
	const int& GetTypeID();

	//生存フラグ
	bool IsDead();
};