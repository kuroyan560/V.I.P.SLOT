#pragma once
#include<memory>
#include"Transform.h"
#include"CollisionCallBack.h"
#include"ColliderParentObject.h"
class Model;
class SlotMachine;
class Camera;
class LightManager;
class Collider;

class Block : public CollisionCallBack, public ColliderParentObject
{
private:
	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {}

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override
	{
		m_hitCount++;
		OnHitTrigger();
	}

protected:
	//�R���C�_�[
	std::weak_ptr<Collider>m_attachCollider;
	//���������̃g�����X�t�H�[��
	Transform m_initTransform;
	//�@���ꂽ��
	int m_hitCount;

	//�����������ŌĂяo�����
	virtual void OnInit() = 0;
	//�X�V�����ŌĂяo�����
	virtual void OnUpdate() = 0;
	//�`�揈���ŌĂяo�����
	virtual void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam) = 0;

	//�@���ꂽ�u�ԌĂяo�����
	virtual void OnHitTrigger() = 0;

public:
	//�g�����X�t�H�[��
	Transform m_transform;
	enum TYPE { COIN, SLOT };

	Block();
	virtual ~Block() {}
	//������
	void Init(Transform& arg_initTransform, std::shared_ptr<Collider>& arg_attachCollider);
	//�X�V
	void Update();
	//�`��
	void Draw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);

	//���S����
	virtual bool IsDead() = 0;
	virtual TYPE GetType() = 0;

	//�R���C�_�[�Q�b�^
	std::shared_ptr<Collider>GetCollider() { return m_attachCollider.lock(); }
};

class CoinBlock : public Block
{
	void OnInit()override {}
	void OnUpdate()override {}
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
public:
	CoinBlock();

	//�R�C�����������Ă��邩�i��x�ł��@����Ă�������������ĂȂ��j
	bool IsEmpty() { return m_hitCount; }

	bool IsDead()override { return false; }
	TYPE GetType()override { return COIN; }
};

class SlotBlock : public Block
{
	//�X���b�g�}�V���|�C���^
	std::weak_ptr<SlotMachine>m_slotMachinePtr;

	void OnInit()override;
	void OnUpdate()override;
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
public:
	SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine);

	bool IsDead()override;
	TYPE GetType()override { return SLOT; }

};