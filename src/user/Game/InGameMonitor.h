#pragma once
#include"Player.h"
#include"CollisionManager.h"

//�Q�[�����̏����Ď�����B�e�����ŕK�v�ȏ��̃|�C���^�𒆌p���鑋���I�����B�p�����Ďg���B
//���������A�Q�ƖړI�݂̂Œl�̕ύX�͕s�\
class InGameMonitor
{
private:
	//�Z�b�g���ꂽ�|�C���^
	static const Player* s_player;
public:
	static void Set(Player* arg_player)
	{
		s_player = arg_player;
	}

protected:
	const Player* GetPlayer()const
	{ 
		if (s_player == nullptr)assert(0);
		return s_player; 
	}
};

