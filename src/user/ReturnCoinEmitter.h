#pragma once
#include"CoinObjectManager.h"
#include"CoinPerform.h"
#include"Transform.h"
#include<forward_list>
class Camera;

class ReturnCoinEmitter
{
	//返却するコインの挙動（スロット上部から放出）
	CoinObjectManager m_returnCoinObjManager;
	class ReturnCoinPerform : public CoinPerform
	{
		float m_fallAccel = 0.0f;
		Vec3<float>m_move;
	public:
		ReturnCoinPerform(Vec3<float>arg_initMove) : m_move(arg_initMove) {	}
		void OnUpdate(Coins& arg_coin)override;
		void OnEmit(Coins& arg_coin)override {};
	};

	struct ReturnCoin
	{
		int m_perCoinNum;
		Transform m_initTransform;
		ReturnCoin(const int& arg_perCoinNum, const Transform& arg_initTransform)
			:m_perCoinNum(arg_perCoinNum), m_initTransform(arg_initTransform) {}
	};
	std::forward_list<ReturnCoin>m_returnCoins;

	//放出タイミング計測
	int m_emitTimer;

	//コイン返却SE
	int m_coinReturnSE;

public:
	ReturnCoinEmitter();

	//初期化
	void Init();
	/// <summary>
	/// 更新
	/// </summary>
	/// <returns>演出を終えた返却するコインの数</returns>
	int Update();
	//描画
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);

	//放出
	void Emit(int arg_coinNumSum, int arg_perCoinNum, const Transform& arg_initTransform);
};

