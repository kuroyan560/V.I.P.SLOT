#pragma once
#include"CollisionCallBack.h"
#include<memory>
#include"Timer.h"
#include"PlayersCounterAttackHitEffect.h"
class CoinVault;
class TexHitEffect;
class ObjectManager;
class GameCamera;
class Player;
class TimeScale;

//�ʏ�U��
class PlayersNormalAttack : public CollisionCallBack
{
	//�U���͂̃|�C���^
	int* m_offensive = nullptr;
	//�v���C���[�̏������̃|�C���^
	CoinVault* m_playersVault = nullptr;
	//�q�b�g�G�t�F�N�g
	std::weak_ptr<TexHitEffect>m_hitEffect;
	//SE
	int m_hitSE;
	int m_killSE;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {};

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;
public:
	PlayersNormalAttack(
		int* arg_offensive,
		CoinVault* arg_playersVault,
		std::weak_ptr<TexHitEffect>arg_hitEffect,
		int arg_hitSE,
		int arg_killSE)
		:m_offensive(arg_offensive), m_playersVault(arg_playersVault), m_hitEffect(arg_hitEffect), m_hitSE(arg_hitSE), m_killSE(arg_killSE) {}
};

//�p���B�U��
class PlayersParryAttack : public CollisionCallBack
{
	//�U���͂̃|�C���^
	int* m_offensive = nullptr;
	//�I�u�W�F�N�g�}�l�[�W���|�C���^�i�p���B�e���˗p�j
	std::weak_ptr<ObjectManager>m_objMgr;
	//�R���W�����}�l�[�W���|�C���^�i�p���B�e���˗p�j
	std::weak_ptr<CollisionManager>m_colMgr;
	//SE
	int m_parrySE;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {};

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;

public:
	PlayersParryAttack(
		int* arg_offensive,
		std::weak_ptr<ObjectManager>arg_objMgr,
		std::weak_ptr<CollisionManager>arg_colMgr,
		int arg_parrySE)
		:m_offensive(arg_offensive), m_objMgr(arg_objMgr), m_colMgr(arg_colMgr), m_parrySE(arg_parrySE) {}
};

//����̍U�����p���B�ɂ���Ď��g�̂��̂ɁA����𗘗p�����U��
class PlayersCounterAttack : public CollisionCallBack
{
	//�U���͂̃|�C���^
	int* m_offensive = nullptr;
	//�v���C���[�̏������̃|�C���^
	CoinVault* m_playersVault = nullptr;
	//�q�b�g�G�t�F�N�g
	PlayersCounterAttackHitEffect* m_hitEffect = nullptr;
	//SE
	int m_hitSE;
	int m_killSE;

	//���̍U�������������G�̐�
	int m_hitCount = 0;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {};

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;
public:
	PlayersCounterAttack(
		int* arg_offensive,
		CoinVault* arg_playersVault,
		PlayersCounterAttackHitEffect* arg_hitEffect,
		int arg_hitSE,
		int arg_killSE)
		:m_offensive(arg_offensive), m_playersVault(arg_playersVault), m_hitEffect(arg_hitEffect), m_hitSE(arg_hitSE), m_killSE(arg_killSE) {}
};

//��_���[�W
class DamagedCallBack : public CollisionCallBack
{
	Player* m_parent;
	std::weak_ptr<GameCamera>m_cam;

	//�q�b�g�X�g�b�v�^�C�}�[
	Timer m_hitStopTimer;

	//���G����
	Timer m_invincibleTimer;

	//���G���Ԓ��_��
	Timer m_flashTimer;
	bool m_isDraw;

	//�q�b�g�X�g�b�vSE
	int m_hitStopSE;
	//�_���[�WSE
	int m_damageSE;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {};

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;

public:
	DamagedCallBack(Player* arg_player, std::weak_ptr<GameCamera>arg_cam, int arg_hitStopSE, int arg_damageSE)
		:m_parent(arg_player), m_cam(arg_cam), m_hitStopSE(arg_hitStopSE), m_damageSE(arg_damageSE) {}
	void Init()
	{
		m_invincibleTimer.Reset(0);
		m_hitStopTimer.Reset(0);
		m_isDraw = true;
	}
	void Update(TimeScale& arg_timeScale);

	const bool& GetIsDraw()const { return m_isDraw; }

	//�f�o�b�O�p�ɊO������Ăяo����悤��
	void Execute();
};