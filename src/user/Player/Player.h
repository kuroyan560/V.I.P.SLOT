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
#include"BasicDrawParameters.h"
#include"PlayersAbility.h"
#include"PlayerHp.h"
#include"PlayersCoinUI.h"
#include"Debugger.h"
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
class TextureBuffer;

class Player : public ColliderParentObject, public Debugger
{
	//HP�Ǘ�
	PlayerHp m_playerHp;

	//�\�͒l
	PlayersAbility m_ability;

	//���f���I�u�W�F�N�g
	std::shared_ptr<ModelObject>m_modelObj;

	//�g�D�[���̃p�����[�^
	IndividualDrawParameter m_drawParam;

	//1�t���[���O�̍��W
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
	//������UI
	PlayersCoinUI m_coinUI;

	//���f���S�̂𕢂���
	std::shared_ptr<CollisionSphere>m_bodySphereCol;

	//�R���C�_�[
	std::shared_ptr<Collider>m_bodyCollider;

	//�q�b�g�G�t�F�N�g
	std::shared_ptr<TexHitEffect>m_hitEffect;

	//���ˍU���ɂ��q�b�g�G�t�F�N�g
	std::shared_ptr<PlayersCounterAttackHitEffect>m_counterHitEffect;

	/*--- �R�[���o�b�N�֐� ---*/
	//�U��
	std::shared_ptr<PlayersNormalAttack>m_normalAttackCallBack;
	//�p���B�U��
	std::shared_ptr<PlayersParryAttack>m_parryAttackCallBack;
	//�p���B�ɂ�锽�ˍU��
	std::shared_ptr<PlayersCounterAttack>m_counterAttackCallBack;
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

	void OnImguiItems()override;
	
public:
	Player();

	void Awake(std::weak_ptr<CollisionManager>arg_collisionMgr, std::weak_ptr<ObjectManager>arg_objMgr, std::weak_ptr<GameCamera>arg_cam);

	//������
	void Init(PlayersAbility arg_ability, int arg_initRemainLife, int arg_initCoinNum);
	//�X�V
	void Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale);
	//�`��
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);
	//�G�t�F�N�g�`��
	void Draw2D(std::weak_ptr<Camera>arg_cam);

	//����Ƃ̓����蔻��
	void HitCheckWithScaffold(const std::weak_ptr<Scaffold>arg_scaffold);

	//�v���C���[�̃��f�������ɍ��킹�����W�Q�b�^
	Vec3<float>GetCenterPos()const;

	//�_���[�W���󂯂�
	void Damage() { m_playerHp.Damage(); }

	//�Q�b�^
	//�U������
	bool IsAttack()const;	
	//������
	const int GetCoinNum()const { return m_coinVault.GetNum(); }
	//���ˍU���̃R�[���o�b�N�i�p���B�e�̃R�[���o�b�N�Ƃ��ė��p���邽�߃Q�b�^��p�Ӂj
	std::weak_ptr<CollisionCallBack>GetCounterAttackCallBack() { return m_counterAttackCallBack; }
	//�c�胉�C�t
	const int& GetLife()const { return m_playerHp.GetLife(); }
	//���S������
	bool IsDead()const { return m_playerHp.IsDead(); }
};