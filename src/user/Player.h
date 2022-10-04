#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
#include"Timer.h"
#include<vector>
#include"CollisionCallBack.h"
#include"Transform.h"
class ModelObject;
class LightManager;
class Camera;
class SlotMachine;
class TimeScale;
class Collider;
class CollisionManager;
class GameCamera;

class Player
{
	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject>m_modelObj;

	//�ړ�
	Vec3<float>m_move = { 0,0,0 };

	//�������x
	float m_fallSpeed;

	//�ڒn�t���O
	bool m_isOnGround;
	//�W�����vSE
	int m_jumpSE;

	//������
	CoinVault m_coinVault;

	//BET�̃X�p���v���p�^�C�}�[
	Timer m_betTimer;
	//�A��BET�̌v���p�^�C�}�[
	Timer m_consecutiveBetTimer;
	//BET��SE
	int m_betSE;

	//�R���C�_�[
	std::shared_ptr<Collider>m_bodyCollider;
	std::shared_ptr<Collider>m_footCollider;

	//HP
	int m_hp;

	/*--- �R�[���o�b�N�֐� ---*/
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

		//�G���݂��\�ɂȂ�܂ł̎���
		Timer m_canStepTimer;

		//�q�b�g�X�g�b�vSE
		int m_hitStopSE;
		//�_���[�WSE
		int m_damageSE;

		void OnCollision(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const unsigned char& arg_otherAttribute,
			const CollisionManager& arg_collisionMgr)override;

	public:
		DamagedCallBack(Player* arg_player, int arg_hitStopSE, int arg_damageSE)
			:m_parent(arg_player), m_hitStopSE(arg_hitStopSE), m_damageSE(arg_damageSE) {}
		void Init(std::weak_ptr<GameCamera>arg_cam)
		{
			m_cam = arg_cam;
			m_invincibleTimer.Reset(0);
			m_hitStopTimer.Reset(0);
			m_canStepTimer.Reset(0);
			m_isDraw = true;
		}
		void Update(TimeScale& arg_timeScale);

		const bool& GetIsDraw()const { return m_isDraw; }
		bool IsCanStep()const { return m_canStepTimer.IsTimeUp(); }
	};
	std::shared_ptr<DamagedCallBack>m_damegedCallBack;

	//�G�ւ̍U��
	class AttackCallBack : public CollisionCallBack
	{
		Player* m_parent;
		//���݂�SE
		int m_stepEnemySE;
		//���G����
		Timer m_invincibleTimer;

		void OnCollision(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider,
			const unsigned char& arg_otherAttribute,
			const CollisionManager& arg_collisionMgr)override;

	public:
		AttackCallBack(Player* arg_player, int arg_stepEnemySE)
			:m_parent(arg_player), m_stepEnemySE(arg_stepEnemySE) {}
		void Init()
		{
			m_invincibleTimer.Reset(0);
		}
		void Update(float arg_timeScale)
		{
			m_invincibleTimer.UpdateTimer(arg_timeScale);
		}
		bool GetIsInvincible()const { return !m_invincibleTimer.IsTimeUp(); }
	};
	std::shared_ptr<AttackCallBack>m_attackCallBack;

	void Jump();
	
public:
	Player(std::weak_ptr<CollisionManager>arg_collisionMgr);
	//������
	void Init(std::weak_ptr<GameCamera>arg_cam);
	//�X�V
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//�R�C��GET�i�{n�\���j
	void GetCoin(int arg_coinNum);
	void GetCoin(CoinVault& arg_coinVault)
	{
		arg_coinVault.Pass(m_coinVault);
		GetCoin(0);
	}
	void GetCoin(CoinVault& arg_coinVault, int arg_coinNum)
	{
		arg_coinVault.Pass(m_coinVault, arg_coinNum);
		GetCoin(0);
	}

	//�R�C����n��
	void PassCoin(CoinVault& arg_coinVault)
	{
		m_coinVault.Pass(arg_coinVault);
	}
	void PassCoin(CoinVault& arg_coinVault, int arg_coinNum)
	{
		m_coinVault.Pass(arg_coinVault, arg_coinNum);
	}

	//�g�����X�t�H�[���Q�b�^
	const Transform& GetTransform()const;
};