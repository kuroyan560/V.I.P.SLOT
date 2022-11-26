#pragma once
#include"Vec.h"
#include<array>
#include<string>

//�����p�p�����[�^
namespace ConstParameter
{
	namespace Environment
	{
		//�X�e�[�W�T�C�Y�i�c���E�����j
		extern Vec2<float> FIELD_FLOOR_SIZE;
		//�X�e�[�W�����W
		extern Vec3<float>FIELD_FLOOR_POS;
		//�X�e�[�W����ʂ̍���
		extern float FIELD_FLOOR_TOP_SURFACE_HEIGHT;
		//�t�B�[���h��X����
		extern float FIELD_WIDTH;
		//�t�B�[���h��Y�����
		extern float FIELD_HEIGHT_MAX;
		//�t�B�[���h��Y������
		extern float FIELD_HEIGHT_MIN;

		void ImGuiDebug();
	};

	namespace Player
	{
		//�����ʒu
		extern Vec3<float>INIT_POS;

		//���f���T�C�Y
		extern Vec3<float>MODEL_SIZE;

		//�ő僉�C�t
		extern int MAX_LIFE;
		//�Q�[���t���[�J�n���̔\�͒l��̍ő僉�C�t
		extern int START_ABILITY_MAX_LIFE;

		//�ő�HP
		extern int MAX_HP;

		/*--- �ړ� ---*/
		//�ړ����x
		extern float MOVE_SPEED;
		//�U�����̈ړ����x
		extern float MOVE_SPEED_WHILE_THROW;
		//�ړ��̓����v�Z��LerpRate
		extern float MOVE_LERP_RATE;

		/*--- �W�����v ---*/
		extern float JUMP_POWER;

		/*--- ���� ---*/
		//���f�����W�����v �� ������艺�~
		//�W�����v���̏d��
		extern float GRAVITY_WHILE_JUMP;
		//�������̏d��
		extern float GRAVITY_WHILE_FALL;
		//�U�����̏d��
		extern float GRAVITY_WHILE_ATTACK;
		//�������x�̉���
		extern float FALL_SPEED_MIN;

		/*--- �Փ˔��� ---*/
		//�_���[�W���󂯂��Ƃ��̃q�b�g�X�g�b�v����
		extern int HIT_STOP_TIME_ON_DAMAGED;
		//�_���[�W���󂯂��Ƃ��̗������x
		extern float FALL_SPEED_ON_DAMAGED;
		//�_���[�W���󂯂���̖��G����
		extern int INVINCIBLE_TIME_ON_DAMAGED;
		//�_���[�W���󂯂���̖��G���Ԓ��̓_�ŃX�p��
		extern int FLASH_SPAN_ON_DAMAGED_INVINCIBLE;
		//�_���[�W���󂯂���A���݂��\�ɂȂ�܂ł̎���
		extern int CAN_STEP_TIME_AFTER_DAMAGED;

		void ImGuiDebug();
	};

	namespace Slot
	{
		//�G��
		enum struct PATTERN { NONE, NUM };

		/*--- �X���b�g ---*/
		//�X���b�g���I����Ă��玟���J�n�o����悤�ɂȂ�܂ł̎���
		extern int SLOT_WAIT_TIME;

		/*--- ���[����] ---*/
		//�ō����x�ɂȂ�܂ł̎���
		extern int UNTIL_MAX_SPEED_TIME;
		//�ō���]���x
		extern float MAX_SPIN_SPEED;
		//��]�I����̃��[���U������
		extern int FINISH_SPIN_TIME;
		//��]�I�����̃��[���U���ő�
		extern float FINISH_SPIN_SHAKE_MAX;

		/*--- �X���b�g�Q�[�W ---*/
		//���߂���{�s�񐔂̍ő�
		extern int SLOT_GAUGE_MAX;

		/*--- �������� ---*/
		enum AUTO_OPERATE
		{
			UNTIL_FIRST_REEL,	//�ŏ��̃��[����~�܂ł̎���
			REEL_STOP_SPAN,	//���[�����Ƃ̒�~�܂ł̎���
			AFTER_STOP_ALL_REEL,	//�S���[����~��̑҂�����
			AUTO_OPERATE_NUM
		};
		//��������̎��ԊԊu
		extern std::array<float, AUTO_OPERATE_NUM>AUTO_OPERATE_TIME;
		//��������̃^�C���X�P�[���ő�
		extern float AUTO_OPERATE_TIME_SCALE_MAX;
		//��������^�C���X�P�[�����f�t�H���g�l�i1.0f�j�ł���X���b�g�Q�[�W��
		extern int DEFAULT_TIME_SCALE_SLOT_GAUGE_NUM;
		//��������^�C���X�P�[�����ő��iAUTO_OPERATE_TIME_SCALE_MAX�j�ł���X���b�g�Q�[�W��
		extern int MAX_TIME_SCALE_SLOT_GAUGE_NUM;		// < SLOT_GAUGE_MAX

		/*--- ���K�z�� ---*/
		extern Vec3<float>MEGA_PHONE_POS;

		void ImGuiDebug();
	};

	namespace GameObject
	{
		//���
		enum struct TYPE
		{
			SLIDE_ENEMY,
			SLIME_BATTERY_ENEMY, 
			ENEMY_BULLET,
			PARRY_BULLET,
			NUM 
		};

		//��ʂ��Ƃ̃R���C�_�[�U�镑������
		extern std::array<std::string, static_cast<int>(TYPE::NUM)>COLLIDER_ATTRIBUTE;

		//��ʂ��Ƃ̍ő吔
		extern std::array<int, static_cast<int>(TYPE::NUM)>INSTANCE_NUM_MAX;

		//�G�������Ԃ�X�����W��Βl
		extern float POS_X_ABS;
		//�G��������Y�����W�ŏ��ƍő�
		extern float POS_Y_MIN;
		extern float POS_Y_MAX;

		/*--- �Փ˔��� ---*/
		//�_���[�W���󂯂���̖��G����
		extern int INVINCIBLE_TIME_ON_DAMAGED;
		//��_���[�W���̉��~����
		extern int OFFSET_Y_TIME_ON_DAMAGED;

		void ImGuiDebug();
	}

	namespace Stage
	{
		//�c���̍ő�u���b�N��
		extern Vec2<int>MAX_BLOCK_NUM_AXIS;
		//�c�������Ȃ��̍ő�u���b�N��
		extern int MAX_BLOCK_NUM;
		//�u���b�N�̈�Ӓ���
		extern float BLOCK_LEN;
		//�u���b�N�̈�Ӓ�������
		extern float BLOCK_LEN_HALF;

		//����̍ő吔
		extern int MAX_SCAFFOLD_NUM;

		void ImGuiDebug();
	}

	void ImguiDebug();
};

