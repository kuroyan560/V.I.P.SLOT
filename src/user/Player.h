#pragma once
#include<string>
#include<memory>
#include"Vec.h"
#include"CoinVault.h"
#include"Timer.h"
#include<vector>
#include"CollisionCallBack.h"
#include"Transform.h"
#include"ColliderParentObject.h"
class ModelObject;
class LightManager;
class Camera;
class SlotMachine;
class TimeScale;
class Collider;
class CollisionManager;
class GameCamera;
class CollisionSphere;
class YoYo;

class Player : public ColliderParentObject
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

	//���f���S�̂𕢂���
	std::shared_ptr<CollisionSphere>m_bodySphereCol;

	//�R���C�_�[
	std::shared_ptr<Collider>m_bodyCollider;

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
	};
	std::shared_ptr<DamagedCallBack>m_damegedCallBack;

	//�u���b�N�ɐG�ꂽ�ۂ̃W�����v���񕜃R�[���o�b�N
	class CallBackWithBlock : public CollisionCallBack
	{
		Player* m_parent;
		std::weak_ptr<CollisionManager>m_collisionMgr;

		void OnCollisionEnter(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider)override {};

		void OnCollisionTrigger(
			const Vec3<float>& arg_inter,
			std::weak_ptr<Collider>arg_otherCollider)override;
	public:
		CallBackWithBlock(Player* arg_player, const std::weak_ptr<CollisionManager>& arg_collisionMgr)
			:m_parent(arg_player), m_collisionMgr(arg_collisionMgr) {}
	};
	std::shared_ptr<CallBackWithBlock>m_callBackWithBlock;

	//�W�����v
	void Jump(Vec3<float>* arg_rockOnPos = nullptr);

	//���삪�L�[�{�[�h���R���g���[���[��
	enum struct INPUT_CONFIG { KEY_BOARD, CONTROLLER };
	INPUT_CONFIG m_inputConfig = INPUT_CONFIG::CONTROLLER;

	//���[���[
	std::shared_ptr<YoYo>m_yoYo;
	
public:
	Player(std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<GameCamera>arg_cam);

	//������
	void Init(int arg_initHp,int arg_initCoinNum);
	//�X�V
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);
	//�G�t�F�N�g�`��
	void EffectDraw(std::weak_ptr<Camera>arg_cam);

	//�R�C���Q�b�g�֐�
	void GetCoin(int arg_coinNum)
	{
		m_coinVault.Add(arg_coinNum);
	}

	//imgui�f�o�b�O
	void ImguiDebug();

	//�v���C���[�̃��f�������ɍ��킹�����W�Q�b�^
	Vec3<float>GetCenterPos()const;

	//�Q�b�^
	const int GetCoinNum()const { return m_coinVault.GetNum(); }
	const int GetHp()const { return m_hp; }
};