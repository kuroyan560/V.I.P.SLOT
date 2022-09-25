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
	};

	namespace Slot
	{
		//�ō����x�ɂȂ�܂ł̎���
		const int UNTIL_MAX_SPEED_TIME = 14;
		//�ō���]���x
		const float MAX_SPIN_SPEED = -0.0065f;
	};
};

