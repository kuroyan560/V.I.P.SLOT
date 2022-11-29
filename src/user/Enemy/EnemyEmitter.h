#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
class ObjectManager;
class CollisionManager;
class TimeScale;
class EnemyEmitter
{
	enum TYPE { SLIDE_MOVE, SLIME_BATTERY, NUM };

	//�o�����i���j
	std::array<int, TYPE::NUM>m_appearRate = { 70,30 };
	//�o�������鎞�ԊԊu
	int m_appearSpan = 180;
	//��x�ɏo�������鐔
	int m_appearNum = 3;

	//���Ԍv��
	Timer m_timer;

	void EmitEnemy(std::weak_ptr<ObjectManager>& arg_objMgr, std::weak_ptr<CollisionManager>& arg_colMgr, int arg_type);
	void EmitEnemys(std::weak_ptr<ObjectManager>& arg_objMgr, std::weak_ptr<CollisionManager>& arg_colMgr);
	void UpdateAppearRate(int arg_fixRateTypeIdx);

public:
	void Init(std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<CollisionManager> arg_colMgr);

	//�e�X�g�p�ɓK���ɓG���o��������
	void TestRandEmit(const TimeScale& arg_timeScale, std::weak_ptr<ObjectManager>arg_objMgr, std::weak_ptr<CollisionManager>arg_colMgr);

	//�e�X�g�o���̃p�����[�^����
	void ImguiDebug();
};