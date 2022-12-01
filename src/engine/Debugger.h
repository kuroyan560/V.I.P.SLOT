#pragma once
#include<string>
#include<vector>
#include"ImguiApp.h"
//imgui�f�o�b�O�̃C���^�[�t�F�[�X
class Debugger
{
private:
	//���蓖�Ă鎯�ʔԍ�
	static int s_id;
	//�N�����̃f�o�b�K�z��
	static std::vector<Debugger*>s_debuggerArray;

public:
	virtual ~Debugger() {}
	//�f�o�b�O�@�\�\��
	static void Draw();
	//�f�o�b�K�o�^
	static void Register(std::vector<Debugger*>arg_debuggerArray)
	{
		s_debuggerArray = arg_debuggerArray;
	}
	//�f�o�b�K�o�^����
	static void ClearRegister() { s_debuggerArray.clear(); }

private:
	//���ʔԍ�
	int m_id;
	//�A�N�e�B�u���
	bool m_active = false;

protected:
	//imgui�E�B���h�E��
	std::string m_title;
	Debugger(std::string arg_title) :m_title(arg_title), m_id(s_id++) {}

	//imgui�̍��� Begin ~ End �ԂɌĂяo������
	virtual void OnImguiItems() = 0;
};