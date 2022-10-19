#pragma once
#include"CoinObjectManager.h"
#include"CoinPerform.h"
#include"Transform.h"
#include<forward_list>
#include"Timer.h"
#include"ValueMinMax.h"
class Camera;

class ReturnCoinEmitter
{
	//返却するコインの挙動（スロット上部から放出）
	CoinObjectManager m_returnCoinObjManager;
	class ReturnCoinPerform : public CoinPerform
	{
		//落下加速度
		float m_fallAccel = 0.0f;
		//移動量
		Vec3<float>m_move;
		//コインにかかる重力
		const float m_coinGravity;

	public:
		ReturnCoinPerform(Vec3<float>arg_initMove, float arg_coinGravity)
			: m_move(arg_initMove),m_coinGravity(arg_coinGravity) {	}
		void OnUpdate(Coins& arg_coin, float arg_timeScale)override;
		void OnEmit(Coins& arg_coin)override {};
		bool IsDead(Coins& arg_coin)override;
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
	Timer m_emitTimer;

	//コイン返却SE
	int m_coinReturnSE;

	//一度に放出するコイン枚数
	int m_emitOnceCoinCountMin;
	int m_emitOnceCoinCountMax;

	//コインにかかる重力
	float m_coinGravity;

public:
	ReturnCoinEmitter();

	//初期化
	void Init(
		float arg_coinEmitSpan,
		int arg_emitOnceCoinCountMin,
		int arg_emitOnceCoinCountMax,
		float arg_coinGravity);
	/// <summary>
	/// 更新
	/// </summary>
	/// <returns>演出を終えた返却するコインの数</returns>
	int Update(float arg_timeScale);
	//描画
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);

	//放出
	void Emit(int arg_coinNumSum, int arg_perCoinNum, const Transform& arg_initTransform);
};

