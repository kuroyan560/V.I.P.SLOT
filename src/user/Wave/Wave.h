#pragma once
#include"ConstParameters.h"
#include<list>
#include"Timer.h"

struct Wave
{
	//�m���}
	int m_norma = 10;

	//���o����񕜃L�b�g�̌�
	int m_healKitNum = 3;

	struct AppearEnemyInfo
	{
		//�G�̎��
		ConstParameter::Enemy::ENEMY_TYPE m_type;
		//�����ʒu
		Vec3<float>m_initPos;
		//�o��^�C�~���O
		float m_appearTiming;
		//��莞�Ԃ��Ƃɏo��
		bool m_loopAppear;
	};
	std::list<AppearEnemyInfo>m_appearInfoList;
};

