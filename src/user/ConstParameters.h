#pragma once
#include"Vec.h"

//調整用パラメータ
namespace ConstParameter
{
	namespace Environment
	{
		const float FIELD_WIDTH_HALF = 30.0f;
	}

	namespace Player
	{
		const Vec3<float>INIT_POS = { 0,0,-20 };
		const float MOVE_SPEED = 0.3f;
		const float MOVE_LERP_RATE = 0.3f;
		const float JUMP_POWER = 0.62f;
		const float STRONG_GRAVITY = 0.34f;
		const float WEAK_GRAVITY = 0.00005f;
		const float FALL_SPEED_MIN = -0.2f;
	};
};

