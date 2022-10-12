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
		const float FIELD_WIDTH = 60.0f;
		//�X�e�[�W�����̔���
		const float FIELD_WIDTH_HALF = FIELD_WIDTH / 2.0f;
		//�t�B�[���h�ʒu��Z��
		const float FIELD_DEPTH = -20.0f;
		//�t�B�[���h�ʒu��Z���Ƃ̃I�t�Z�b�g�i���f���̈ʒu�Y���j
		const float FIELD_DEPTH_MODEL_OFFSET = -5.2f;
		//�t�B�[���h�ʒu��Z���i�I�t�Z�b�g�K�p�j
		const float FIELD_DEPTH_FIXED = FIELD_DEPTH + FIELD_DEPTH_MODEL_OFFSET;
		//�t�B�[���h�̍���
		const float FIELD_HEIGHT = 32.5f;
		//�R�C���ɂ�����d��
		const float COIN_GRAVITY = -0.002f;
		//�t�B�[���h���̍���
		const float FLOOR_HEIGHT = 2.6f;
		//�t�B�[���h���̍����̔���
		const float FLOOR_HEIGHT_HALF = FLOOR_HEIGHT / 2.0f;
	}

	namespace Player
	{
		//�����ʒu
		const Vec3<float>INIT_POS = { 0,0,Environment::FIELD_DEPTH };

		//���f�������ɍ��킹�邽�߂̃I�t�Z�b�g�l
		const Vec3<float>FIX_MODEL_CENTER_OFFSET = { 0.0f,2.0f,ConstParameter::Environment::FIELD_DEPTH_MODEL_OFFSET };

		//�ő�HP
		const int MAX_HP = 10;

		/*--- �ړ� ---*/
		//�ړ����x
		const float MOVE_SPEED = 0.3f;
		//�ړ��̓����v�Z��LerpRate
		const float MOVE_LERP_RATE = 0.3f;

		/*--- �W�����v ---*/
		//�W�����v��
		const float JUMP_POWER = 0.62f;

		/*--- ���� ---*/
		//���f�����W�����v �� ������艺�~
		//�W�����v���̏d��
		const float STRONG_GRAVITY = 0.34f;
		//�������̏d��
		const float WEAK_GRAVITY = 0.00005f;
		//�������x�̉���
		const float FALL_SPEED_MIN = -0.2f;

		/*--- �R�C��BET ---*/
		//�ō���BET�ɓ��B����܂ł̎��ԁi�������ł��񂾂�BET�̊Ԋu���Z���Ȃ��Ă����j
		const int UNTIL_MAX_SPEED_BET_TIME = 160;
		//�ŒᑬBET�̃X�p��
		const int BET_SPEED_MAX_SPAN = 4;
		//�ō���BET�̃X�p��
		const int BET_SPEED_MIN_SPAN = 31;
		//��x�ɓ�������R�C���̐�
		const int PASS_COIN_NUM = 1;

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

		/*--- BET���ꂽ�R�C�� ---*/
		//�������Ă���R�C����BET�����܂ł̎���
		const int UNTIL_THROW_COIN_TO_BET = 10;
		//�R�C���������̈ʒu�i������ꂽ�R�C���̌�������j
		const Vec3<float>COIN_PORT_POS = { -17.0f,13.0f,-10.0f };

		/*--- ���K�z�����o ---*/
		//���K�z���ʒu
		const Vec3<float>MEGA_PHONE_POS = { -18.0f,5.0f,20.0f };
		//BET���̃��K�z���g�k�X�P�[��
		const float MEGA_PHONE_EXPAND_SCALE = 1.2f;
		//BET���̃��K�z���g�k����
		const int MEGA_PHONE_EXPAND_TIME = 60;

		/*--- �R�C���ԋp ---*/
		//�X���b�g�ʒu�i�ԋp�R�C���̕��o�ʒu�j
		const Vec3<float>SLOT_POS_ON_BACK_CAM = { 0.0f,18.0f,22.0f };
		//�`���̕ԋp�R�C���̕��o�X�p��
		const int RETURN_COIN_EMIT_SPAN = 6;
		//���̒l�ɂ��ԋp�R�C��1���`��
		const int RETURN_COIN_DRAW_NUM_PER = 2;
		//�`���̕ԋp�R�C���̕��o������
		const int EMIT_COIN_COUNT_MIN = 1;
		//�`���̕ԋp�R�C���̕��o�����
		const int EMIT_COIN_COUNT_MAX = 10;
	};

	namespace GameObject
	{
		//���
		enum struct TYPE { WEAK_SLIDE, NUM };

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

		//�G�������Ԃ�Y�����W����
		const float POS_Y_MIN = Environment::FLOOR_HEIGHT_HALF;
		//�G�������Ԃ�Y�����W���
		const float POS_Y_MAX = Environment::FIELD_HEIGHT;

		//�G�������Ԃ�Z�����W
		const float POS_Z = Environment::FIELD_DEPTH_FIXED;

		/*--- �Փ˔��� ---*/
		//�_���[�W���󂯂���̖��G����
		const int INVINCIBLE_TIME_ON_DAMAGED = 20;
		//��_���[�W���̉��~����
		const int OFFSET_Y_TIME_ON_DAMAGED = 15;
	}

	namespace Stage
	{
		namespace Block
		{
			enum struct TYPE { COIN, SLOT, NUM };
		}
	}
};

