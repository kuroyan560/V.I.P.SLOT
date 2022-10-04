#pragma once
#include<memory>
#include<array>
#include<forward_list>
#include"ConstParameters.h"
#include"CoinPerform.h"
#include"CoinObjectManager.h"

class EnemyBreed;
class Enemy;
class TimeScale;
class LightManager;
class Camera;
class CollisionManager;
class CoinVault;

//�G�l�~�[�̒�`�A�Ǘ����s��
class EnemyManager
{
	using ENEMY_TYPE = ConstParameter::Enemy::TYPE;

	//�G�����񂾂�R�C���𗎂Ƃ�
	CoinObjectManager m_dropCoinObjManager;
	class DropCoinPerform : public CoinPerform
	{
		float m_fallAccel = 0.0f;
		Vec3<float>m_move;
	public:
		DropCoinPerform(Vec3<float>arg_initMove) : m_move(arg_initMove) { m_move.z = 0.0f; }
		void OnUpdate(Coins& arg_coin, float arg_timeScale)override;
		void OnEmit(Coins& arg_coin)override {};
	};

	//����
	std::array<std::shared_ptr<EnemyBreed>, static_cast<int>(ENEMY_TYPE::NUM)>m_breeds;

	//�S�G�l�~�[�z��
	std::array<std::forward_list<std::shared_ptr<Enemy>>, static_cast<int>(ENEMY_TYPE::NUM)>m_enemys;

	//���S���Ă���G�l�~�[�z��
	std::array<std::forward_list<std::shared_ptr<Enemy>>, static_cast<int>(ENEMY_TYPE::NUM)>m_deadEnemyArray;
	//�������Ă���G�l�~�[�z��
	std::array<std::forward_list<std::shared_ptr<Enemy>>, static_cast<int>(ENEMY_TYPE::NUM)>m_aliveEnemyArray;

	//�v���C���[�ɃR�C����������Ƃ���SE
	int m_dropCoinReturnSE;

	//�G�̓o�ꎞ�ɌĂяo��
	void OnEnemyAppear(std::shared_ptr<Enemy>& arg_enemy, std::weak_ptr<CollisionManager>arg_collisionMgr);
	//�G�̎��S���ɌĂяo��
	void OnEnemyDead(std::shared_ptr<Enemy>& arg_enemy, std::weak_ptr<CollisionManager>arg_collisionMgr, bool arg_dropCoin);
public:
	EnemyManager();
	void Init(std::weak_ptr<CollisionManager>arg_collisionMgr);
	void Update(const TimeScale& arg_timeScale, std::weak_ptr<CollisionManager>arg_collisionMgr, CoinVault& arg_playerVault);
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	void Appear(ENEMY_TYPE arg_type, std::weak_ptr<CollisionManager>arg_collisionMgr);
};

