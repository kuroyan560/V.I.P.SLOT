#include "ConstParameters.h"
#include"imgui.h"
#include<magic_enum.h>
#include<functional>
#include<array>

namespace ConstParameter
{
    namespace Environment
    {
        //�X�e�[�W�T�C�Y�i�c���E�����j
        Vec2<float>FIELD_FLOOR_SIZE = { 60.0f,1.0f };
        //�X�e�[�W�����W
        Vec3<float>FIELD_FLOOR_POS = { 0.0f,-2.0f,-25.0f };
        //�X�e�[�W����ʂ̍���
        float FIELD_FLOOR_TOP_SURFACE_HEIGHT = FIELD_FLOOR_POS.y + FIELD_FLOOR_SIZE.y / 2.0f;
        //�t�B�[���h��X����
        float FIELD_WIDTH = 64.0f;
        //�t�B�[���h��Y�����
        float FIELD_HEIGHT_MAX = 32.0f;
        //�t�B�[���h��Y������
        float FIELD_HEIGHT_MIN = -1.0f;
    };

    namespace Player
    {
        //�����ʒu
        Vec3<float>INIT_POS = { 0,Environment::FIELD_FLOOR_TOP_SURFACE_HEIGHT,Environment::FIELD_FLOOR_POS.z };

        //���f���T�C�Y
        Vec3<float>MODEL_SIZE = { 1.0f,2.0f,1.0f };

        //�ő�HP
        int MAX_HP = 10;

        /*--- �ړ� ---*/
        //�ړ����x
        float MOVE_SPEED = 0.45f;
        //�U�����̈ړ����x
        float MOVE_SPEED_WHILE_ATTACK = MOVE_SPEED * 0.3f;
        //�ړ��̓����v�Z��LerpRate
        float MOVE_LERP_RATE = 0.3f;

        /*--- �W�����v ---*/
        //�W�����v��
        float JUMP_POWER = 0.62f;

        /*--- ���� ---*/
        //���f�����W�����v �� ������艺�~
        //�W�����v���̏d��
        float STRONG_GRAVITY = 0.34f;
        //�������̏d��
        float WEAK_GRAVITY = 0.00005f;
        //�������x�̉���
        float FALL_SPEED_MIN = -0.2f;

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
