#include "ConstParameters.h"
#include"imgui.h"
#include<magic_enum.h>
#include<functional>
#include<array>

namespace ConstParameter
{
    namespace Environment
    {
        //�X�e�[�W����
        float FIELD_WIDTH = 60.0f;
        //�t�B�[���h�ʒu��Z��
        float FIELD_DEPTH = -20.0f;
        //�t�B�[���h�ʒu��Z���Ƃ̃I�t�Z�b�g�i���f���̈ʒu�Y���j
        float FIELD_DEPTH_MODEL_OFFSET = -5.2f;
        //�t�B�[���h�ʒu��Z���i�I�t�Z�b�g�K�p�j
        float FIELD_DEPTH_FIXED = FIELD_DEPTH + FIELD_DEPTH_MODEL_OFFSET;
        //�t�B�[���h��Y�����
        float FIELD_HEIGHT_MAX = 25.5f;
        //�t�B�[���h��Y������
        float FIELD_HEIGHT_MIN = 0.0f;
    };

    namespace Player
    {
        //�����ʒu
        Vec3<float>INIT_POS = { 0,0,Environment::FIELD_DEPTH };

        //���f�������ɍ��킹�邽�߂̃I�t�Z�b�g�l
        Vec3<float>FIX_MODEL_CENTER_OFFSET = { 0.0f,2.0f,ConstParameter::Environment::FIELD_DEPTH_MODEL_OFFSET };

        //�ő�HP
        int MAX_HP = 10;

        /*--- �ړ� ---*/
        //���͂ɂ������x����
        float INPUT_ACCEL_POWER = 0.25f;
        //���͂ɂ������x�ω���
        float ACCEL_LERP_RATE = 0.8f;
        //�ړ��ʂ̌����ω���
        float MOVE_DAMP_RATE = 0.3f;

        /*--- �Փ˔��� ---*/
        //�_���[�W���󂯂��Ƃ��̃q�b�g�X�g�b�v����
        int HIT_STOP_TIME_ON_DAMAGED = 30;
        //�_���[�W���󂯂��Ƃ��̗������x
        float FALL_SPEED_ON_DAMAGED = -1.0f;
        //�_���[�W���󂯂���̖��G����
        int INVINCIBLE_TIME_ON_DAMAGED = 120;
        //�_���[�W���󂯂���̖��G���Ԓ��̓_�ŃX�p��
        int FLASH_SPAN_ON_DAMAGED_INVINCIBLE = 4;
        //�_���[�W���󂯂���A���݂��\�ɂȂ�܂ł̎���
        int CAN_STEP_TIME_AFTER_DAMAGED = 20;
    }

    namespace Slot
    {
        /*--- �X���b�g ---*/
        //�X���b�g���I����Ă��玟���J�n�o����悤�ɂȂ�܂ł̎���
        int SLOT_WAIT_TIME = 13;

        /*--- ���[����] ---*/
        //�ō����x�ɂȂ�܂ł̎���
        int UNTIL_MAX_SPEED_TIME = 14;
        //�ō���]���x
        float MAX_SPIN_SPEED = -0.0065f;
        //��]�I����̃��[���U������
        int FINISH_SPIN_TIME = 30;
        //��]�I�����̃��[���U���ő�
        float FINISH_SPIN_SHAKE_MAX = 0.02f;

        /*--- ���K�z�� ---*/
        Vec3<float>MEGA_PHONE_POS = { 0.0f,18.0f,22.0f };
    }

    namespace GameObject
    {
        //��ʂ��Ƃ̃R���C�_�[�U�镑������
        std::array<std::string, static_cast<int>(TYPE::NUM)>COLLIDER_ATTRIBUTE =
        {
            "Enemy"
        };

        //��ʂ��Ƃ̍ő吔
        std::array<int, static_cast<int>(TYPE::NUM)>INSTANCE_NUM_MAX =
        {
            100
        };

        //�G�������Ԃ�X�����W��Βl
        float POS_X_ABS = 43.0f;

        /*--- �Փ˔��� ---*/
        //�_���[�W���󂯂���̖��G����
        int INVINCIBLE_TIME_ON_DAMAGED = 20;
        //��_���[�W���̉��~����
        int OFFSET_Y_TIME_ON_DAMAGED = 15;
    }

    namespace Stage
    {
        //�c���̍ő�u���b�N��
        Vec2<int>MAX_BLOCK_NUM_AXIS = { 20,15 };
        //�c�������Ȃ��̍ő�u���b�N��
        int MAX_BLOCK_NUM = MAX_BLOCK_NUM_AXIS.x * MAX_BLOCK_NUM_AXIS.y;
        //�u���b�N�̈�Ӓ���
        float BLOCK_LEN = 2.0f;
        //�u���b�N�̈�Ӓ�������
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
