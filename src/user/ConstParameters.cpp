#include "ConstParameters.h"
#include"imgui.h"
#include<magic_enum.h>
#include<functional>
#include<array>

namespace ConstParameter
{
    namespace Environment
    {
        //ステージサイズ（縦幅・横幅）
        Vec2<float>FIELD_FLOOR_SIZE = { 60.0f,1.0f };
        //ステージ床座標
        Vec3<float>FIELD_FLOOR_POS = { 0.0f,-2.0f,-25.0f };
        //ステージ床上面の高さ
        float FIELD_FLOOR_TOP_SURFACE_HEIGHT = FIELD_FLOOR_POS.y + FIELD_FLOOR_SIZE.y / 2.0f;
        //フィールドのX軸幅
        float FIELD_WIDTH = 64.0f;
        //フィールドのY軸上限
        float FIELD_HEIGHT_MAX = 32.0f;
        //フィールドのY軸下限
        float FIELD_HEIGHT_MIN = -1.0f;
    };

    namespace Player
    {
        //初期位置
        Vec3<float>INIT_POS = { 0,Environment::FIELD_FLOOR_TOP_SURFACE_HEIGHT,Environment::FIELD_FLOOR_POS.z };

        //モデルサイズ
        Vec3<float>MODEL_SIZE = { 1.0f,2.0f,1.0f };

        //最大HP
        int MAX_HP = 10;

        /*--- 移動 ---*/
        //移動速度
        float MOVE_SPEED = 0.45f;
        //攻撃中の移動速度
        float MOVE_SPEED_WHILE_ATTACK = MOVE_SPEED * 0.3f;
        //移動の動き計算のLerpRate
        float MOVE_LERP_RATE = 0.3f;

        /*--- ジャンプ ---*/
        //ジャンプ力
        float JUMP_POWER = 0.62f;

        /*--- 落下 ---*/
        //※素早くジャンプ → ゆっくり下降
        //ジャンプ中の重力
        float STRONG_GRAVITY = 0.34f;
        //落下中の重力
        float WEAK_GRAVITY = 0.00005f;
        //落下速度の下限
        float FALL_SPEED_MIN = -0.2f;

        /*--- 衝突判定 ---*/
        //ダメージを受けたときのヒットストップ時間
        int HIT_STOP_TIME_ON_DAMAGED = 30;
        //ダメージを受けたときの落下速度
        float FALL_SPEED_ON_DAMAGED = -1.0f;
        //ダメージを受けた後の無敵時間
        int INVINCIBLE_TIME_ON_DAMAGED = 120;
        //ダメージを受けた後の無敵時間中の点滅スパン
        int FLASH_SPAN_ON_DAMAGED_INVINCIBLE = 4;
        //ダメージを受けた後、踏みつけ可能になるまでの時間
        int CAN_STEP_TIME_AFTER_DAMAGED = 20;
    }

    namespace Slot
    {
        /*--- スロット ---*/
        //スロットが終わってから次を開始出来るようになるまでの時間
        int SLOT_WAIT_TIME = 13;

        /*--- リール回転 ---*/
        //最高速度になるまでの時間
        int UNTIL_MAX_SPEED_TIME = 14;
        //最高回転速度
        float MAX_SPIN_SPEED = -0.0065f;
        //回転終了後のリール振動時間
        int FINISH_SPIN_TIME = 30;
        //回転終了時のリール振動最大
        float FINISH_SPIN_SHAKE_MAX = 0.02f;

        /*--- メガホン ---*/
        Vec3<float>MEGA_PHONE_POS = { 0.0f,18.0f,22.0f };
    }

    namespace GameObject
    {
        //種別ごとのコライダー振る舞い名称
        std::array<std::string, static_cast<int>(TYPE::NUM)>COLLIDER_ATTRIBUTE =
        {
            "Enemy"
        };

        //種別ごとの最大数
        std::array<int, static_cast<int>(TYPE::NUM)>INSTANCE_NUM_MAX =
        {
            100
        };

        //敵がいる空間のX軸座標絶対値
        float POS_X_ABS = 43.0f;

        /*--- 衝突判定 ---*/
        //ダメージを受けた後の無敵時間
        int INVINCIBLE_TIME_ON_DAMAGED = 20;
        //被ダメージ時の下降時間
        int OFFSET_Y_TIME_ON_DAMAGED = 15;
    }

    namespace Stage
    {
        //縦横の最大ブロック数
        Vec2<int>MAX_BLOCK_NUM_AXIS = { 20,15 };
        //縦横分けなしの最大ブロック数
        int MAX_BLOCK_NUM = MAX_BLOCK_NUM_AXIS.x * MAX_BLOCK_NUM_AXIS.y;
        //ブロックの一辺長さ
        float BLOCK_LEN = 2.0f;
        //ブロックの一辺長さ半分
        float BLOCK_LEN_HALF = BLOCK_LEN / 2.0f;
    }
};


void ConstParameter::ImguiDebug()
{
    static enum NAME_SPACE
    {
        Environment,
        Player,
        Slot,
        GameObject,
        Stage,
        Num
    }selectNameSpace = Environment;

    static std::array<std::function<void()>, static_cast<int>(NAME_SPACE::Num)> imguiFuncs =
    {
        Environment::ImGuiDebug,
        Player::ImGuiDebug,
        Slot::ImGuiDebug,
        GameObject::ImGuiDebug,
        Stage::ImGuiDebug
    };


    ImGui::Begin("ConstParameters", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("namespace"))
        {
            for (int i = 0; i < NAME_SPACE::Num; ++i)
            {
                NAME_SPACE space = static_cast<NAME_SPACE>(i);
                if (ImGui::MenuItem(std::string(magic_enum::enum_name(space)).c_str()))
                {
                    selectNameSpace = space;
                }
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    auto nowSpaceName = std::string(magic_enum::enum_name(selectNameSpace));
    ImGui::Text("%s", nowSpaceName.c_str());
    imguiFuncs[static_cast<int>(selectNameSpace)]();

    ImGui::End();
}

void ConstParameter::Environment::ImGuiDebug()
{
    ImGui::DragFloat("FIELD_WIDTH", &FIELD_WIDTH, 0.5f);
    ImGui::DragFloat("FIELD_HEIGHT_MAX", &FIELD_HEIGHT_MAX, 0.5f);
    ImGui::DragFloat("FIELD_HEIGHT_MIN", &FIELD_HEIGHT_MIN, 0.5f);
}

void ConstParameter::Player::ImGuiDebug()
{
}

void ConstParameter::Slot::ImGuiDebug()
{
}

void ConstParameter::GameObject::ImGuiDebug()
{
}

void ConstParameter::Stage::ImGuiDebug()
{
}
