#pragma once
#include"ConstParameters.h"
#include<list>
#include"Timer.h"

struct Wave
{
	//ノルマ
	int m_norma = 10;

	//放出する回復キットの個数
	int m_healKitNum = 3;

	struct AppearEnemyInfo
	{
		//敵の種別
		ConstParameter::Enemy::ENEMY_TYPE m_type;
		//初期位置
		Vec3<float>m_initPos;
		//登場タイミング
		float m_appearTiming;
		//一定時間ごとに出現
		bool m_loopAppear;
	};
	std::list<AppearEnemyInfo>m_appearInfoList;
};

