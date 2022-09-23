#pragma once
#include"Vec.h"
class UsersInput;

//�{�^�����蓖�ėp�^�O
enum struct HANDLE_INPUT_TAG
{
	ATTACK,			//�ʏ�U��
	JUMP,							//�W�����v
	GUARD_DODGE_RUN,		// �K�[�h / ��� / �_�b�V��
	MARKING,						//�}�[�L���O
	RUSH,							//���b�V��
	ABILITY,							//�A�r���e�B����
	GENERIC_ACTION,			//�ėp�A�N�V����
	NUM,	//�^�O��
	NONE,
};

//���蓖�Ă���{�^��
enum struct CAN_ALLOCATE_BUTTON
{
	A, B, Y, X, LB, RB, LT, RT, NUM,
};

//�{�^�����蓖�Đݒ�
class ControllerConfig
{
	int m_controllerIdx = 0;
	HANDLE_INPUT_TAG m_allocateButton[static_cast<int>(CAN_ALLOCATE_BUTTON::NUM)];
	Vec2<bool>m_camMirror = { false,false };

	int GetAllocateButtonIdx(HANDLE_INPUT_TAG Tag)const;
	CAN_ALLOCATE_BUTTON GetAllocateButton(HANDLE_INPUT_TAG Tag)const
	{
		return static_cast<CAN_ALLOCATE_BUTTON>(GetAllocateButtonIdx(Tag));
	}

public:
	ControllerConfig() { Reset(); }
	//�����ݒ�
	void Reset();

	//���͎擾
	bool GetHandleInput(const UsersInput& Input, HANDLE_INPUT_TAG Tag)const;
	bool GetHandleInput(const UsersInput& Input, int TagIdx)const
	{
		return GetHandleInput(Input, static_cast<HANDLE_INPUT_TAG>(TagIdx));
	}

	//�ړ�����
	Vec2<float>GetMoveVec(const UsersInput& Input)const;
	//�J�����������
	Vec2<float>GetCameraVec(const UsersInput& Input)const;
	//�J�������b�N�I������
	bool GetCameraRock(const UsersInput& Input)const;
	//�\���{�^���i���E�Fx �A�㉺�Fy �� -1,0,1 �̂����ꂩ�j
	Vec2<int>GetDpadTrigger(const UsersInput& Input)const;

	//�R���g���[���[��U��������
	void ShakeController(UsersInput& Controller, float Power, int Span);

	//�{�^�������蓖�Ă�i���X���蓖�Ă��Ă������̂Ɠ���ւ��j
	void ReAllocateButton(HANDLE_INPUT_TAG Tag, CAN_ALLOCATE_BUTTON AllocateButton);
	//�J�����[�̃~���[�����O�ݒ�
	void ResetCamMirrorX(bool MirrorX) { m_camMirror.x = MirrorX; }
	void ResetCamMirrorY(bool MirrorY) { m_camMirror.y = MirrorY; }

	//Imgui�f�o�b�O�@�\
	void ImguiDebug(UsersInput& Input);
};
