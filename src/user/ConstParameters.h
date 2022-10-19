#pragma once
#include"Vec.h"
#include<array>
#include<string>

//�����p�p�����[�^
namespace ConstParameter
{
	namespace Environment
	{
		//�X�e�[�W����
		extern float FIELD_WIDTH;
		//�t�B�[���h�ʒu��Z��
		const float FIELD_DEPTH = -20.0f;
		//�t�B�[���h�ʒu��Z���Ƃ̃I�t�Z�b�g�i���f���̈ʒu�Y���j
		const float FIELD_DEPTH_MODEL_OFFSET = -5.2f;
		//�t�B�[���h�ʒu��Z���i�I�t�Z�b�g�K�p�j
		const float FIELD_DEPTH_FIXED = FIELD_DEPTH + FIELD_DEPTH_MODEL_OFFSET;
		//�t�B�[���h��Y�����
		extern float FIELD_HEIGHT_MAX;
		//�t�B�[���h��Y������
		extern float FIELD_HEIGHT_MIN;

		void ImGuiDebug();
	};

	namespace Player
	{
		//�����ʒu
		const Vec3<float>INIT_POS = { 0,0,Environment::FIELD_DEPTH };

		//���f�������ɍ��킹�邽�߂̃I�t�Z�b�g�l
		const Vec3<float>FIX_MODEL_CENTER_OFFSET = { 0.0f,2.0f,ConstParameter::Environment::FIELD_DEPTH_MODEL_OFFSET };

		//�ő�HP
		const int MAX_HP = 10;

		/*--- �ړ� ---*/
		//���͂ɂ������x����
		const float INPUT_ACCEL_POWER = 0.25f;
		//���͂ɂ������x�ω���
		const float ACCEL_LERP_RATE = 0.8f;
		//�ړ��ʂ̌����ω���
		const float MOVE_DAMP_RATE = 0.3f;

		/*--- �Փ˔��� ---*/
		//�_���[�W���󂯂��Ƃ��̃q�b�g�X�g�b�v����
		const int HIT_STOP_TIME_ON_DAMAGED = 30;
		//�_���[�W���󂯂��Ƃ��̗������x
		const float FALL_SPEED_ON_DAMAGED = -1.0f;
		//�_���[�W���󂯂���̖��G����
		const int INVINCIBLE_TIME_ON_DAMAGED = 120;
		//�_���[�W���󂯂���̖��G���Ԓ��̓_�ŃX�p��
		const int FLASH_SPAN_ON_DAMAGED_INVINCIBLE = 4;
		//�_���[�W���󂯂���A���݂��\�ɂȂ�܂ł̎���
		const int CAN_STEP_TIME_AFTER_DAMAGED = 20;

		void ImGuiDebug();
	};

	namespace Slot
	{
		//�G��
		enum struct PATTERN { NONE, NUM };

		/*--- �X���b�g ---*/
		//�X���b�g���I����Ă��玟���J�n�o����悤�ɂȂ�܂ł̎���
		const int SLOT_WAIT_TIME = 13;

		/*--- ���[����] ---*/
		//�ō����x�ɂȂ�܂ł̎���
		const int UNTIL_MAX_SPEED_TIME = 14;
		//�ō���]���x
		const float MAX_SPIN_SPEED = -0.0065f;
		//��]�I����̃��[���U������
		const int FINISH_SPIN_TIME = 30;
		//��]�I�����̃��[���U���ő�
		const float FINISH_SPIN_SHAKE_MAX = 0.02f;

		/*--- ���K�z�� ---*/
		const Vec3<float>MEGA_PHONE_POS = { 0.0f,18.0f,22.0f };

		void ImGuiDebug();
	};

	namespace GameObject
	{
		//���
		enum struct TYPE { ENEMY, NUM };

		//��ʂ��Ƃ̃R���C�_�[�U�镑������
		const std::array<std::string, static_cast<int>(TYPE::NUM)>COLLIDER_ATTRIBUTE =
		{
			"Enemy"
		};

		//��ʂ��Ƃ̍ő吔
		const std::array<int, static_cast<int>(TYPE::NUM)>INSTANCE_NUM_MAX =
		{
			100
		};

		//�G�������Ԃ�X�����W��Βl
		const float POS_X_ABS = 43.0f;

		/*--- �Փ˔��� ---*/
		//�_���[�W���󂯂���̖��G����
		const int INVINCIBLE_TIME_ON_DAMAGED = 20;
		//��_���[�W���̉��~����
		const int OFFSET_Y_TIME_ON_DAMAGED = 15;

		void ImGuiDebug();
	}

	namespace Stage
	{
		//�c���̍ő�u���b�N��
		static const Vec2<int>MAX_BLOCK_NUM_AXIS = { 20,15 };
		//�c�������Ȃ��̍ő�u���b�N��
		static const int MAX_BLOCK_NUM = MAX_BLOCK_NUM_AXIS.x * MAX_BLOCK_NUM_AXIS.y;
		//�u���b�N�̈�Ӓ���
		const float BLOCK_LEN = 2.0f;
		//�u���b�N�̈�Ӓ�������
		const float BLOCK_LEN_HALF = BLOCK_LEN / 2.0f;

		void ImGuiDebug();
	}

	void ImguiDebug();
};

