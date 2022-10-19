#pragma once
#include"Vec.h"
#include<array>
#include<string>

//調整用パラメータ
namespace ConstParameter
{
	namespace Environment
	{
		//ステージ横幅
		extern float FIELD_WIDTH;
		//フィールド位置のZ軸
		const float FIELD_DEPTH = -20.0f;
		//フィールド位置のZ軸とのオフセット（モデルの位置ズレ）
		const float FIELD_DEPTH_MODEL_OFFSET = -5.2f;
		//フィールド位置のZ軸（オフセット適用）
		const float FIELD_DEPTH_FIXED = FIELD_DEPTH + FIELD_DEPTH_MODEL_OFFSET;
		//フィールドのY軸上限
		extern float FIELD_HEIGHT_MAX;
		//フィールドのY軸下限
		extern float FIELD_HEIGHT_MIN;

		void ImGuiDebug();
	};

	namespace Player
	{
		//初期位置
		const Vec3<float>INIT_POS = { 0,0,Environment::FIELD_DEPTH };

		//モデル中央に合わせるためのオフセット値
		const Vec3<float>FIX_MODEL_CENTER_OFFSET = { 0.0f,2.0f,ConstParameter::Environment::FIELD_DEPTH_MODEL_OFFSET };

		//最大HP
		const int MAX_HP = 10;

		/*--- 移動 ---*/
		//入力による加速度強さ
		const float INPUT_ACCEL_POWER = 0.25f;
		//入力による加速度変化率
		const float ACCEL_LERP_RATE = 0.8f;
		//移動量の減衰変化率
		const float MOVE_DAMP_RATE = 0.3f;

		/*--- 衝突判定 ---*/
		//ダメージを受けたときのヒットストップ時間
		const int HIT_STOP_TIME_ON_DAMAGED = 30;
		//ダメージを受けたときの落下速度
		const float FALL_SPEED_ON_DAMAGED = -1.0f;
		//ダメージを受けた後の無敵時間
		const int INVINCIBLE_TIME_ON_DAMAGED = 120;
		//ダメージを受けた後の無敵時間中の点滅スパン
		const int FLASH_SPAN_ON_DAMAGED_INVINCIBLE = 4;
		//ダメージを受けた後、踏みつけ可能になるまでの時間
		const int CAN_STEP_TIME_AFTER_DAMAGED = 20;

		void ImGuiDebug();
	};

	namespace Slot
	{
		//絵柄
		enum struct PATTERN { NONE, NUM };

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

		/*--- メガホン ---*/
		const Vec3<float>MEGA_PHONE_POS = { 0.0f,18.0f,22.0f };

		void ImGuiDebug();
	};

	namespace GameObject
	{
		//種別
		enum struct TYPE { ENEMY, NUM };

		//種別ごとのコライダー振る舞い名称
		const std::array<std::string, static_cast<int>(TYPE::NUM)>COLLIDER_ATTRIBUTE =
		{
			"Enemy"
		};

		//種別ごとの最大数
		const std::array<int, static_cast<int>(TYPE::NUM)>INSTANCE_NUM_MAX =
		{
			100
		};

		//敵がいる空間のX軸座標絶対値
		const float POS_X_ABS = 43.0f;

		/*--- 衝突判定 ---*/
		//ダメージを受けた後の無敵時間
		const int INVINCIBLE_TIME_ON_DAMAGED = 20;
		//被ダメージ時の下降時間
		const int OFFSET_Y_TIME_ON_DAMAGED = 15;

		void ImGuiDebug();
	}

	namespace Stage
	{
		//縦横の最大ブロック数
		static const Vec2<int>MAX_BLOCK_NUM_AXIS = { 20,15 };
		//縦横分けなしの最大ブロック数
		static const int MAX_BLOCK_NUM = MAX_BLOCK_NUM_AXIS.x * MAX_BLOCK_NUM_AXIS.y;
		//ブロックの一辺長さ
		const float BLOCK_LEN = 2.0f;
		//ブロックの一辺長さ半分
		const float BLOCK_LEN_HALF = BLOCK_LEN / 2.0f;

		void ImGuiDebug();
	}

	void ImguiDebug();
};

