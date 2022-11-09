#pragma once
#include"Vec.h"
#include<array>
#include<string>

//調整用パラメータ
namespace ConstParameter
{
	namespace Environment
	{
		//ステージサイズ（縦幅・横幅）
		extern Vec2<float> FIELD_FLOOR_SIZE;
		//ステージ床座標
		extern Vec3<float>FIELD_FLOOR_POS;
		//ステージ床上面の高さ
		extern float FIELD_FLOOR_TOP_SURFACE_HEIGHT;
		//フィールドのX軸幅
		extern float FIELD_WIDTH;
		//フィールドのY軸上限
		extern float FIELD_HEIGHT_MAX;
		//フィールドのY軸下限
		extern float FIELD_HEIGHT_MIN;

		void ImGuiDebug();
	};

	namespace Player
	{
		//初期位置
		extern Vec3<float>INIT_POS;

		//モデルサイズ
		extern Vec3<float>MODEL_SIZE;

		//最大HP
		extern int MAX_HP;

		/*--- 移動 ---*/
		//移動速度
		extern float MOVE_SPEED;
		//攻撃中の移動速度
		extern float MOVE_SPEED_WHILE_THROW;
		//移動の動き計算のLerpRate
		extern float MOVE_LERP_RATE;

		/*--- ジャンプ ---*/
		extern float JUMP_POWER;

		/*--- 落下 ---*/
		//※素早くジャンプ → ゆっくり下降
		//ジャンプ中の重力
		extern float GRAVITY_WHILE_JUMP;
		//落下中の重力
		extern float GRAVITY_WHILE_FALL;
		//攻撃中の重力
		extern float GRAVITY_WHILE_ATTACK;
		//落下速度の下限
		extern float FALL_SPEED_MIN;

		/*--- 衝突判定 ---*/
		//ダメージを受けたときのヒットストップ時間
		extern int HIT_STOP_TIME_ON_DAMAGED;
		//ダメージを受けたときの落下速度
		extern float FALL_SPEED_ON_DAMAGED;
		//ダメージを受けた後の無敵時間
		extern int INVINCIBLE_TIME_ON_DAMAGED;
		//ダメージを受けた後の無敵時間中の点滅スパン
		extern int FLASH_SPAN_ON_DAMAGED_INVINCIBLE;
		//ダメージを受けた後、踏みつけ可能になるまでの時間
		extern int CAN_STEP_TIME_AFTER_DAMAGED;

		void ImGuiDebug();
	};

	namespace Slot
	{
		//絵柄
		enum struct PATTERN { NONE, NUM };

		/*--- スロット ---*/
		//スロットが終わってから次を開始出来るようになるまでの時間
		extern int SLOT_WAIT_TIME;

		/*--- リール回転 ---*/
		//最高速度になるまでの時間
		extern int UNTIL_MAX_SPEED_TIME;
		//最高回転速度
		extern float MAX_SPIN_SPEED;
		//回転終了後のリール振動時間
		extern int FINISH_SPIN_TIME;
		//回転終了時のリール振動最大
		extern float FINISH_SPIN_SHAKE_MAX;

		/*--- メガホン ---*/
		extern Vec3<float>MEGA_PHONE_POS;

		void ImGuiDebug();
	};

	namespace GameObject
	{
		//種別
		enum struct TYPE { ENEMY, NUM };

		//種別ごとのコライダー振る舞い名称
		extern std::array<std::string, static_cast<int>(TYPE::NUM)>COLLIDER_ATTRIBUTE;

		//種別ごとの最大数
		extern std::array<int, static_cast<int>(TYPE::NUM)>INSTANCE_NUM_MAX;

		//敵がいる空間のX軸座標絶対値
		extern float POS_X_ABS;

		/*--- 衝突判定 ---*/
		//ダメージを受けた後の無敵時間
		extern int INVINCIBLE_TIME_ON_DAMAGED;
		//被ダメージ時の下降時間
		extern int OFFSET_Y_TIME_ON_DAMAGED;

		void ImGuiDebug();
	}

	namespace Stage
	{
		//縦横の最大ブロック数
		extern Vec2<int>MAX_BLOCK_NUM_AXIS;
		//縦横分けなしの最大ブロック数
		extern int MAX_BLOCK_NUM;
		//ブロックの一辺長さ
		extern float BLOCK_LEN;
		//ブロックの一辺長さ半分
		extern float BLOCK_LEN_HALF;

		void ImGuiDebug();
	}

	void ImguiDebug();
};

