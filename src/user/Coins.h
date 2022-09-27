#pragma once
#include"Transform.h"
#include"Timer.h"
class Coins
{
public:
	//�R�C����
	int m_coinNum;
	//���������̃g�����X�t�H�[��
	const Transform m_initTransform;
	//���g�̃g�����X�t�H�[��
	Transform m_transform;
	//��������
	Timer m_timer;
	//���S�t���O
	bool m_isDead = false;

	Coins(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime)
		:m_coinNum(arg_coinNum), m_initTransform(arg_initTransform), m_transform(m_initTransform)
	{
		m_timer.Reset(arg_lifeTime);
	}
};