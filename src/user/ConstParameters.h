#pragma once
#include"Vec.h"

//調整用パラメータ
namespace ConstParameter
{
	namespace Environment
	{
		//ステージ横幅の半分
		const float FIELD_WIDTH_HALF = 30.0f;
	}

	namespace Player
	{
		//初期位置
		const Vec3<float>INIT_POS = { 0,0,-20 };
		//移動速度
		const float MOVE_SPEED = 0.3f;
		//移動の動き計算のLerpRate
		const float MOVE_LERP_RATE = 0.3f;

		//ジャンプ力
		const float JUMP_POWER = 0.62f;

		//※素早くジャンプ → ゆっくり下降
		//ジャンプ中の重力
		const float STRONG_GRAVITY = 0.34f;
		//落下中の重力
		const float WEAK_GRAVITY = 0.00005f;

		//落下速度の下限
		const float FALL_SPEED_MIN = -0.2f;

		//最高速BETに到達するまでの時間（長押しでだんだんBETの間隔が短くなっていく）
		const int UNTIL_MAX_SPEED_BET_TIME = 160;
		//最低速BETのスパン
		const int BET_SPEED_MAX_SPAN = 3;
		//最高速BETのスパン
		const int BET_SPEED_MIN_SPAN = 30;
		//一度に投入するコインの数
		const int PASS_COIN_NUM = 1;
	};

	namespace Slot
	{
		//スロットが終わってから次を開始出来るようになるまでの時間
		const int SLOT_WAIT_TIME = 12;

		//最高速度になるまでの時間
		const int UNTIL_MAX_SPEED_TIME = 14;
		//最高回転速度
		const float MAX_SPIN_SPEED = -0.0065f;

		//回転終了後のリール振動時間
		const int FINISH_SPIN_TIME = 30;
		//回転終了時のリール振動最大
		const float FINISH_SPIN_SHAKE_MAX = 0.02f;

		//投げられてからコインがBETされるまでの時間
		const int UNTIL_THROW_COIN_TO_BET = 10;
		//コイン投入口の位置
		const Vec3<float>COIN_PORT_POS = { -15.0f,14.0f,-10.0f };
	};
};

