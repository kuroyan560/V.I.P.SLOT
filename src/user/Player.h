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
#include"PlayersCallBack.h"
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
class Scaffold;
class ObjectManager;

class Player : public ColliderParentObject
{
	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject>m_modelObj;

	Vec3<float>m_oldPos;
	//�ړ�
	Vec3<float>m_move = { 0,0,0 };

	//�������x
	float m_fallSpeed;

	//�ڒn�t���O�i�n�ʁj
	bool m_isOnGround;
	//�ڒn�t���O�i����j
	bool m_isOnScaffold;

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

	//�U����
	int m_offensive = 1;

	//�q�b�g�G�t�F�N�g
	std::shared_ptr<TexHitEffect>m_hitEffect;

	/*--- �R�[���o�b�N�֐� ---*/
	//�U��
	std::shared_ptr<PlayersNormalAttack>m_normalAttackCallBack;
	//�p���B�U��
	std::shared_ptr<PlayersParryAttack>m_parryAttackCallBack;
	//��_���[�W
	std::shared_ptr<DamagedCallBack>m_damegedCallBack;

	//���삪�L�[�{�[�h���R���g���[���[��
	enum struct INPUT_CONFIG { KEY_BOARD, CONTROLLER };
	INPUT_CONFIG m_inputConfig = INPUT_CONFIG::CONTROLLER;

	//���[���[
	std::shared_ptr<YoYo>m_yoYo;

	//�����Ă������X
	float m_vecX;

	/* --- ���ꂩ��~��铮�� --- */
	//����Ƃ̓����蔻������Ȃ����Ԍv��
	Timer m_stepDownTimer;
	//���ꂩ��~��Ă��邩
	bool m_stepDown;
	//���ꂩ��~�肽�Ƃ��̗����X�s�[�h
	float m_stepDownFallSpeed = -0.06f;

	//�W�����v
	void Jump(Vec3<float>* arg_rockOnPos = nullptr);

	//�n�ʒ��n���ɌĂяo��
	void OnLanding(bool arg_isGround);
	
public:
	Player(std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<ObjectManager>arg_objMgr, std::weak_ptr<GameCamera>arg_cam);

	//������
	void Init(int arg_initHp,int arg_initCoinNum);
	//�X�V
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);
	//�G�t�F�N�g�`��
	void Draw2D(std::weak_ptr<Camera>arg_cam);

	//�R�C���Q�b�g�֐�
	void GetCoin(int arg_coinNum)
	{
		m_coinVault.Add(arg_coinNum);
	}

	//�_���[�W
	void Damage() { m_hp = std::min(0, m_hp - 1); }

	//imgui�f�o�b�O
	void ImguiDebug();

	//����Ƃ̓����蔻��
	void HitCheckWithScaffold(const std::weak_ptr<Scaffold>arg_scaffold);

	//�v���C���[�̃��f�������ɍ��킹�����W�Q�b�^
	Vec3<float>GetCenterPos()const;

	//�Q�b�^
	const int GetCoinNum()const { return m_coinVault.GetNum(); }
	const int GetHp()const { return m_hp; }
	bool IsAttack()const;
};