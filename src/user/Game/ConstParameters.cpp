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

        //�ő僉�C�t
        //�@��UI�\���I�ɂ͂U�����E�B�Q�[���I�ɂ����ꂭ�炢���ȁH
        int MAX_LIFE = 6;

        //�Q�[���t���[�J�n���̔\�͒l��̍ő僉�C�t
        //int START_ABILITY_MAX_LIFE = 1;
        int START_ABILITY_MAX_LIFE = 3;
        //�Q�[���t���[�J�n���̔\�͒l��̍U����
        int START_ABILITY_OFFSENSIVE = 1;

        //�ő�HP
        int MAX_HP = 5;

        /*--- �ړ� ---*/
        //�ړ����x
        float MOVE_SPEED = 0.45f;
        //�U�����̈ړ����x
        float MOVE_SPEED_WHILE_THROW = MOVE_SPEED * 0.3f;
        //�ړ��̓����v�Z��LerpRate
        float MOVE_LERP_RATE = 0.3f;

        /*--- �W�����v ---*/
        //�W�����v��
        float JUMP_POWER = 0.62f;

        /*--- ���� ---*/
        //���f�����W�����v �� ������艺�~
        //�W�����v���̏d��
        float GRAVITY_WHILE_JUMP = 0.34f;
        //�������̏d��
        float GRAVITY_WHILE_FALL = 0.0015f;
        //�U�����̏d��
        float GRAVITY_WHILE_ATTACK = GRAVITY_WHILE_FALL * 0.4f;

        //�������x�̉���
        float FALL_SPEED_MIN = -0.2f;

        /*--- �Փ˔��� ---*/
        //�_���[�W���󂯂��Ƃ��̃q�b�g�X�g�b�v����
        int HIT_STOP_TIME_ON_DAMAGED = 10;
        //�_���[�W���󂯂ă��C�t(�n�[�g)������Ƃ��̃q�b�g�X�g�b�v����
        int HIT_STOP_TIME_ON_DAMAGED_CONSUME_LIFE = 30;
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

        /*--- �X���b�g�Q�[�W ---*/
        //���߂���{�s�񐔂̍ő�
        int SLOT_GAUGE_MAX = 12;

        /*--- �������� ---*/
        //����̎��ԊԊu
        std::array<float, AUTO_OPERATE_NUM>AUTO_OPERATE_TIME =
        {
            120.0f,
            60.0f,
            120.0f
        };
        //��������̃^�C���X�P�[���ő�
        float AUTO_OPERATE_TIME_SCALE_MAX = 1.5f;
        //��������^�C���X�P�[�����f�t�H���g�l�i1.0f�j�ł���X���b�g�Q�[�W��
        int DEFAULT_TIME_SCALE_SLOT_GAUGE_NUM = 3;
        //��������^�C���X�P�[�����ő��iAUTO_OPERATE_TIME_SCALE_MAX�j�ł���X���b�g�Q�[�W��
        int MAX_TIME_SCALE_SLOT_GAUGE_NUM = 9;		// < SLOT_GAUGE_MAX

        /*--- ���K�z�� ---*/
        Vec3<float>MEGA_PHONE_POS = { 0.0f,18.0f,22.0f };
    }

    namespace GameObject
    {
        //��ʂ��Ƃ̃R���C�_�[�U�镑������
        std::array<std::string, static_cast<int>(OBJ_TYPE::NUM)>COLLIDER_ATTRIBUTE =
        {
            "Enemy","Enemy","Enemy","Enemy_Attack","Player_Attack",
        };

        //��ʂ��Ƃ̍ő吔
        std::array<int, static_cast<int>(OBJ_TYPE::NUM)>INSTANCE_NUM_MAX =
        {
            30,30,30,50,50
        };

        //�G�������Ԃ�X�����W��Βl
        float POS_X_ABS = 60;
        //�G��������Y�����W�ŏ��ƍő�
        float POS_Y_MIN = Environment::FIELD_HEIGHT_MIN;
        float POS_Y_MAX = Environment::FIELD_HEIGHT_MAX;

        /*--- �Փ˔��� ---*/
        //�_���[�W���󂯂���̖��G����
        int INVINCIBLE_TIME_ON_DAMAGED = 20;
        //��_���[�W���̉��~����
        int OFFSET_Y_TIME_ON_DAMAGED = 15;
    }

    namespace Enemy
    {
        /*--- ���ړ� ---*/
        namespace Slide
        {
            //�x���G�̈ړ��X�s�[�h
            float SLOW_MOVE_SPEED = 0.2f;
            //�����G�̈ړ��X�s�[�h
            float FAST_MOVE_SPEED = 0.4f;
        }

        /*--- �X���C���Œ�C�� ---*/
        namespace SlimeBattery
        {
            //�ҋ@����
            float WAIT_INTERVAL = 60.0f;
            //�W�����v�ɂ����鎞��
            float JUMP_INTERVAL = 60.0f;
            //�W�����v��
            float JUMP_POWER = 0.5f;
        }
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

        //����̍ő吔
        int MAX_SCAFFOLD_NUM = 10;
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
    ImGui::DragFloat("JUMP_POWER", &JUMP_POWER);
    ImGui::DragFloat("GRAVITY_WHILE_JUMP", &GRAVITY_WHILE_JUMP);
    ImGui::DragFloat("GRAVITY_WHILE_FALL", &GRAVITY_WHILE_FALL, 1.0f, 0.0f, 0.0f, "%f");
    ImGui::DragFloat("FALL_SPEED_MIN", &FALL_SPEED_MIN);
}

void ConstParameter::Slot::ImGuiDebug()
{
    ImGui::DragInt("SLOT_GAUGE_MAX", &SLOT_GAUGE_MAX);
    ImGui::DragFloat("AUTO_OPERATE_TIME_SCALE_MAX", &AUTO_OPERATE_TIME_SCALE_MAX);
    ImGui::DragInt("DEFAULT_TIME_SCALE_SLOT_GAUGE_NUM", &DEFAULT_TIME_SCALE_SLOT_GAUGE_NUM);
    ImGui::DragInt("MAX_TIME_SCALE_SLOT_GAUGE_NUM", &MAX_TIME_SCALE_SLOT_GAUGE_NUM);
}

void ConstParameter::GameObject::ImGuiDebug()
{
}

void ConstParameter::Stage::ImGuiDebug()
{
}
