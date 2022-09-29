#pragma once
#include<memory>
#include<forward_list>
#include"Coins.h"
class Model;
class CoinPerform;
class LightManager;
class Camera;

//コイン演出・挙動
class CoinObjectManager
{
private:
	//コイン配列
	std::forward_list<Coins>m_coins;

	//コインモデル
	std::shared_ptr<Model>m_coinModel;

public:
	//挙動をアタッチ
	CoinObjectManager();
	//初期化
	void Init();
	/// <summary>
	/// 更新
	/// </summary>
	/// <returns>演出終了したコインの総数（０ならどのコインも終了していない）</returns>
	int Update(float arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//コイン追加
	void Add(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime, CoinPerform* arg_perform);
};