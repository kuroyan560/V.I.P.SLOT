#pragma once
#include"Vec.h"
#include<array>

//�����p�p�����[�^
namespace ConstParameter
{
	namespace Environment
	{
		//�X�e�[�W�����̔���
		const float FIELD_WIDTH_HALF = 30.0f;
		//�t�B�[���h�ʒu��Z��
		const float FIELD_DEPTH = -20.0f;
		//�t�B�[���h�ʒu��Z���Ƃ̃I�t�Z�b�g�i���f���̈ʒu�Y���j
		const float FIELD_DEPTH_MODEL_OFFSET = -5.2f;
	}

	namespace Player
	{
		//�����ʒu
		const Vec3<float>INIT_POS = { 0,0,Environment::FIELD_DEPTH };

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
		//�_���[�W���󂯂���̖��G����
		const int INVINCIBLE_TIME_WHEN_DAMAGED = 120;
	};

	namespace Slot
	{
		//�G��
		enum struct PATTERN { DOUBLE, TRIPLE, NUM };

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
		//�ԋp�R�C���̏d��
		const float RETURN_COIN_GRAVITY = -0.002f;
		//�ԋp�R�C���̎���
		const int RETURN_COIN_LIFE_TIME = 200;
		//�`���̕ԋp�R�C���̕��o�X�p��
		const int RETURN_COIN_EMIT_SPAN = 6;
		//���̒l�ɂ��ԋp�R�C��1���`��
		const int RETURN_COIN_DRAW_NUM_PER = 2;
		//�`���̕ԋp�R�C���̕��o������
		const int EMIT_COIN_COUNT_MIN = 1;
		//�`���̕ԋp�R�C���̕��o�����
		const int EMIT_COIN_COUNT_MAX = 10;
	};

	namespace Enemy
	{
		//���
		enum struct TYPE { WEAK_SLIDE, NUM };

		//��ʂ��Ƃ̍ő吔
		const std::array<int, static_cast<int>(TYPE::NUM)>INSTANCE_NUM_MAX =
		{
			100
		};

		//�G�������Ԃ�X�����W��Βl
		const float POS_X_ABS = 43.0f;

		//�G�������Ԃ�Y�����W����
		const float POS_Y_MIN = 1.0f;
		//�G�������Ԃ�Y�����W���
		const float POS_Y_MAX = 32.5f;

		//�G�������Ԃ�Z�����W
		const float POS_Z = Environment::FIELD_DEPTH + Environment::FIELD_DEPTH_MODEL_OFFSET;
	}
};

