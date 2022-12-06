#pragma once
#include<memory>
#include"Timer.h"
#include<vector>
#include<array>
#include"Debugger.h"
#include"ConstParameters.h"
#include<map>
#include<forward_list>

class ObjectManager;
class CollisionManager;
class TimeScale;
class GameObject;
class EnemyEmitter : public Debugger
{
	std::weak_ptr<ObjectManager>m_referObjMgr;
	std::weak_ptr<CollisionManager>m_referCollisionMgr;

	//�G�̃I�u�W�F�N�g�|�C���^�z��
	std::forward_list<std::weak_ptr<GameObject>>m_enemyObjList;

	using TYPE = ConstParameter::Enemy::ENEMY_TYPE;
	//�o�����i���j
	std::array<int, static_cast<int>(TYPE::NUM)>m_appearRate;
	//�o�������鎞�ԊԊu
	int m_appearSpan = 180;
	//��x�ɏo�������鐔
	int m_appearNum = 3;

	//���Ԍv��
	Timer m_timer;

	enum { IMGUI_RANDOM_APPEAR, IMGUI_CUSTOM_PARAM }m_imguiDebugMode = IMGUI_CUSTOM_PARAM;

	/*--- �e��G�̃p�����[�^ ---*/
	std::map<TYPE, std::map<std::string, float>>m_customParams;

	void RandEmitEnemy(TYPE arg_type);
	void RandEmitEnemys();
	void UpdateAppearRate(int arg_fixRateTypeIdx);
	void OnImguiItems()override;

public:
	EnemyEmitter();
	void Awake(std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<CollisionManager> arg_colMgr)
	{
		m_referObjMgr = arg_objMgr;
		m_referCollisionMgr = arg_colMgr;
	}
	void Init();

	//�G�̏o��
	void EmitEnemy(TYPE arg_type, Vec3<float>arg_initPos);

	//�e�X�g�p�ɓK���ɓG���o��������
	void TestRandEmit(const TimeScale& arg_timeScale);

	/// <summary>
	/// �S�Ă̓G����|
	/// </summary>
	/// <returns>���Ƃ����R�C�����v</returns>
	int KillAllEnemys();
};