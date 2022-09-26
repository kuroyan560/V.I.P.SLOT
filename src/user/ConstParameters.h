#pragma once
#include"Vec.h"

//�����p�p�����[�^
namespace ConstParameter
{
	namespace Environment
	{
		//�X�e�[�W�����̔���
		const float FIELD_WIDTH_HALF = 30.0f;
	}

	namespace Player
	{
		//�����ʒu
		const Vec3<float>INIT_POS = { 0,0,-20 };
		//�ړ����x
		const float MOVE_SPEED = 0.3f;
		//�ړ��̓����v�Z��LerpRate
		const float MOVE_LERP_RATE = 0.3f;

		//�W�����v��
		const float JUMP_POWER = 0.62f;

		//���f�����W�����v �� ������艺�~
		//�W�����v���̏d��
		const float STRONG_GRAVITY = 0.34f;
		//�������̏d��
		const float WEAK_GRAVITY = 0.00005f;

		//�������x�̉���
		const float FALL_SPEED_MIN = -0.2f;

		//�ō���BET�ɓ��B����܂ł̎��ԁi�������ł��񂾂�BET�̊Ԋu���Z���Ȃ��Ă����j
		const int UNTIL_MAX_SPEED_BET_TIME = 160;
		//�ŒᑬBET�̃X�p��
		const int BET_SPEED_MAX_SPAN = 3;
		//�ō���BET�̃X�p��
		const int BET_SPEED_MIN_SPAN = 30;
		//��x�ɓ�������R�C���̐�
		const int PASS_COIN_NUM = 1;
	};

	namespace Slot
	{
		//�X���b�g���I����Ă��玟���J�n�o����悤�ɂȂ�܂ł̎���
		const int SLOT_WAIT_TIME = 12;

		//�ō����x�ɂȂ�܂ł̎���
		const int UNTIL_MAX_SPEED_TIME = 14;
		//�ō���]���x
		const float MAX_SPIN_SPEED = -0.0065f;

		//��]�I����̃��[���U������
		const int FINISH_SPIN_TIME = 30;
		//��]�I�����̃��[���U���ő�
		const float FINISH_SPIN_SHAKE_MAX = 0.02f;

		//�������Ă���R�C����BET�����܂ł̎���
		const int UNTIL_THROW_COIN_TO_BET = 10;
		//�R�C���������̈ʒu
		const Vec3<float>COIN_PORT_POS = { -15.0f,14.0f,-10.0f };
	};
};

