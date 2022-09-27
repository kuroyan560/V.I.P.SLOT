#pragma once
#include"Transform.h"
#include"Timer.h"
#include<memory>
class CoinPerform;

class Coins
{
	std::unique_ptr<CoinPerform>m_perform;
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

	Coins(int arg_coinNum, const Transform& arg_initTransform, int arg_lifeTime, CoinPerform* arg_perform);
	void Update();
};