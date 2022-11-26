#pragma once
#include<memory>
#include<array>
#include"Vec.h"
#include"Timer.h"
#include<vector>
#include"Transform.h"
#include<forward_list>
class Model;
class LightManager;
class Camera;

//敵を倒してもらえるコインのエフェクト
class EnemyKillCoinEffect
{
	//＋モデルのインデックス
	const int PLUS_IDX = 10;

	//モデルの数
	static const int MODEL_NUM = 11;
	//数字＋モデル
	std::array<std::shared_ptr<Model>, MODEL_NUM>m_numModels;

	struct Info
	{
		//挙動開始時の座標記録用
		Vec3<float>m_startPos;

		//Y軸回転量
		Angle m_angleY = Angle(-360);
		//挙動開始時のY軸回転量記録用
		Angle m_startAngleY = m_angleY;

		//コイン数
		int m_coinNum;

		//エフェクトの親トランスフォーム
		Transform m_transform;

		//数字情報
		struct Number
		{
			//数字モデルインデックス
			int m_numIdx;
			//座標オフセットトランスフォーム
			Transform m_transform;
		};
		std::vector<Number>m_numbers;

		//ステータス
		enum STATUS { APPEAR, WAIT, DISAPPEAR, NUM }m_status = APPEAR;
		//タイマー
		std::array<Timer, STATUS::NUM>m_timer;

	public:
		Info(const EnemyKillCoinEffect& arg_parent, Vec3<float>arg_pos, int arg_coinNum);
		void Update(float arg_timeScale);
		void Draw(const EnemyKillCoinEffect& arg_parent, std::weak_ptr<LightManager>&arg_lightMgr, std::weak_ptr<Camera>&arg_cam);

		bool IsEnd() { return m_status == NUM; }
	};
	std::forward_list<Info>m_infoArray;

public:
	EnemyKillCoinEffect();
	void Init();
	void Update(float arg_timeScale);
	void Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam);

	//エフェクト出現
	void Emit(Vec3<float>arg_playerPos, int arg_coinNum);
};

