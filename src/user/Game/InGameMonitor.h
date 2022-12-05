#pragma once
#include"Player.h"
#include"CollisionManager.h"

//ゲーム内の情報を監視する。各処理で必要な情報のポインタを中継する窓口的役割。継承して使う。
//※ただし、参照目的のみで値の変更は不可能
class InGameMonitor
{
private:
	//セットされたポインタ
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

