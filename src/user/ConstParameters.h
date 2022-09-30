#pragma once
#include"Vec.h"
#include<array>

//調整用パラメータ
namespace ConstParameter
{
	namespace Environment
	{
		//ステージ横幅の半分
		const float FIELD_WIDTH_HALF = 30.0f;
		//フィールド位置のZ軸
		const float FIELD_DEPTH = -20.0f;
		//フィールド位置のZ軸とのオフセット（モデルの位置ズレ）
		const float FIELD_DEPTH_MODEL_OFFSET = -5.2f;
	}

	namespace Player
	{
		//初期位置
		const Vec3<float>INIT_POS = { 0,0,Environment::FIELD_DEPTH };

		//最大HP
		const int MAX_HP = 10;

		/*--- 移動 ---*/
		//移動速度
		const float MOVE_SPEED = 0.3f;
		//移動の動き計算のLerpRate
		const float MOVE_LERP_RATE = 0.3f;

		/*--- ジャンプ ---*/
		//ジャンプ力
		const float JUMP_POWER = 0.62f;

		/*--- 落下 ---*/
		//※素早くジャンプ → ゆっくり下降
		//ジャンプ中の重力
		const float STRONG_GRAVITY = 0.34f;
		//落下中の重力
		const float WEAK_GRAVITY = 0.00005f;
		//落下速度の下限
		const float FALL_SPEED_MIN = -0.2f;

		/*--- コインBET ---*/
		//最高速BETに到達するまでの時間（長押しでだんだんBETの間隔が短くなっていく）
		const int UNTIL_MAX_SPEED_BET_TIME = 160;
		//最低速BETのスパン
		const int BET_SPEED_MAX_SPAN = 4;
		//最高速BETのスパン
		const int BET_SPEED_MIN_SPAN = 31;
		//一度に投入するコインの数
		const int PASS_COIN_NUM = 1;

		/*--- 衝突判定 ---*/
		//ダメージを受けた後の無敵時間
		const int INVINCIBLE_TIME_WHEN_DAMAGED = 120;
	};

	namespace Slot
	{
		//絵柄
		enum struct PATTERN { DOUBLE, TRIPLE, NUM };

		/*--- スロット ---*/
		//スロットが終わってから次を開始出来るようになるまでの時間
		const int SLOT_WAIT_TIME = 13;

		/*--- リール回転 ---*/
		//最高速度になるまでの時間
		const int UNTIL_MAX_SPEED_TIME = 14;
		//最高回転速度
		const float MAX_SPIN_SPEED = -0.0065f;
		//回転終了後のリール振動時間
		const int FINISH_SPIN_TIME = 30;
		//回転終了時のリール振動最大
		const float FINISH_SPIN_SHAKE_MAX = 0.02f;

		/*--- BETされたコイン ---*/
		//投げられてからコインがBETされるまでの時間
		const int UNTIL_THROW_COIN_TO_BET = 10;
		//コイン投入口の位置（投げられたコインの向かう先）
		const Vec3<float>COIN_PORT_POS = { -17.0f,13.0f,-10.0f };

		/*--- メガホン演出 ---*/
		//メガホン位置
		const Vec3<float>MEGA_PHONE_POS = { -18.0f,5.0f,20.0f };
		//BET時のメガホン拡縮スケール
		const float MEGA_PHONE_EXPAND_SCALE = 1.2f;
		//BET時のメガホン拡縮時間
		const int MEGA_PHONE_EXPAND_TIME = 60;

		/*--- コイン返却 ---*/
		//スロット位置（返却コインの放出位置）
		const Vec3<float>SLOT_POS_ON_BACK_CAM = { 0.0f,18.0f,22.0f };
		//返却コインの重力
		const float RETURN_COIN_GRAVITY = -0.002f;
		//返却コインの寿命
		const int RETURN_COIN_LIFE_TIME = 200;
		//描画上の返却コインの放出スパン
		const int RETURN_COIN_EMIT_SPAN = 6;
		//この値につき返却コイン1枚描画
		const int RETURN_COIN_DRAW_NUM_PER = 2;
		//描画上の返却コインの放出数下限
		const int EMIT_COIN_COUNT_MIN = 1;
		//描画上の返却コインの放出数上限
		const int EMIT_COIN_COUNT_MAX = 10;
	};

	namespace Enemy
	{
		//種別
		enum struct TYPE { WEAK_SLIDE, NUM };

		//種別ごとの最大数
		const std::array<int, static_cast<int>(TYPE::NUM)>INSTANCE_NUM_MAX =
		{
			100
		};

		//敵がいる空間のX軸座標絶対値
		const float POS_X_ABS = 43.0f;

		//敵がいる空間のY軸座標下限
		const float POS_Y_MIN = 1.0f;
		//敵がいる空間のY軸座標上限
		const float POS_Y_MAX = 32.5f;

		//敵がいる空間のZ軸座標
		const float POS_Z = Environment::FIELD_DEPTH + Environment::FIELD_DEPTH_MODEL_OFFSET;
	}
};

