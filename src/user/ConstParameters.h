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
	};

	namespace Slot
	{
		//最高速度になるまでの時間
		const int UNTIL_MAX_SPEED_TIME = 14;
		//最高回転速度
		const float MAX_SPIN_SPEED = -0.0065f;
	};
};

