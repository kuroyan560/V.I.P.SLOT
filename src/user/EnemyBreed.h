#pragma once
#include<memory>
#include<string>
#include"Transform.h"
#include"Collider.h"
class Model;
class Enemy;

//�G�l�~�[�s���p�^�[��
class EnemyControl
{
public:
	//������
	virtual void Init() = 0;
	//�X�V
	virtual void Update() = 0;

	//�p����̍U���p�^�[���̃N���[���𐶐�������
	virtual EnemyControl* Clone()const = 0;
};

//ActPoint�̓o�^���
class EnemyActPointInfo
{
public:
	//�������g�����X�t�H�[��
	const Transform m_initTransform;
	//�}�[�L���O�ő��
	const int m_maxMarkingCount = 1;
	//�A�^�b�`����{�[����
	//const std::string m_attachBoneName;

	EnemyActPointInfo(
		const int& MaxMarkingCount, 
		const Transform& InitTransform = Transform(),
		const std::string& AttachBoneName = "")
		:m_maxMarkingCount(MaxMarkingCount), 
		m_initTransform(InitTransform)
	/*	m_attachBoneName(AttachBoneName)*/ {}
};

//�G�l�~�[�̌n���i�^�I�u�W�F�N�g�j�A�X�|�[�i�[�Ƃ��Ă̋@�\������
class EnemyBreed
{
private:
/*--- �R�[���o�b�N�N���[���� ---*/
	//��_���[�W
	class DamgeTakenColliderCallBack : public CollisionCallBack
	{
		friend class EnemyBreed;
		Enemy* m_parent;
		void OnCollision(const Vec3<float>& Inter, const std::weak_ptr<Collider> OtherCollider)override;
	public:
		DamgeTakenColliderCallBack(Enemy* Parent) :m_parent(Parent) {}
		CollisionCallBack* Clone()override
		{
			return new DamgeTakenColliderCallBack(m_parent);
		}
	};
	static DamgeTakenColliderCallBack s_damageCallBack;

public:
	static CollisionCallBack* GetDamageCallBack() { return &s_damageCallBack; }


private:
	//�n����
	const std::string m_name;
	//HP�ő�l
	const int m_maxHp;
	//���f��
	std::shared_ptr<Model>m_model;
	//ActPoint�̓o�^���̔z��
	std::vector<EnemyActPointInfo>m_actPointInfos;
	//�s���p�^�[���̃I���W�i��
	const EnemyControl* m_control = nullptr;
	//���Ƃ̔�������铖���蔻��pCollisionPoint�̍��W�I�t�Z�b�g
	Vec3<float>m_colliderWithFloorOffset;
	//�R���C�_�[�̃N���[����
	std::vector<std::shared_ptr<Collider>>m_collidersOrigin;

public:
	EnemyBreed(
		const std::string& Name,
		const int& Hp,
		const std::shared_ptr<Model>& Model,
		const std::vector<EnemyActPointInfo>& ActPointInfos,
		const std::vector<std::shared_ptr<Collider>>& ColliderOrigin,
		const Vec3<float>& ColliderWithFloorOffset = { 0,0,0 },
		EnemyControl* Control = nullptr)
		:m_name(Name),
		m_maxHp(Hp),
		m_model(Model),
		m_actPointInfos(ActPointInfos), 
		m_collidersOrigin(ColliderOrigin),
		m_colliderWithFloorOffset(ColliderWithFloorOffset),
		m_control(Control) {}
	~EnemyBreed() { delete m_control; m_control = nullptr; }

	//�Q�b�^
	const std::shared_ptr<Model>& GetModel()const { return m_model; }
	const int& GetMaxHp()const { return m_maxHp; }

	//�G�̐���
	std::shared_ptr<Enemy>Generate();
};