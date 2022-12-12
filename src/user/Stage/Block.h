#pragma once
#include<memory>
#include"Transform.h"
#include"CollisionCallBack.h"
#include"ColliderParentObject.h"
#include"Timer.h"
class Model;
class SlotMachine;
class Camera;
class LightManager;
class Collider;
class TexHitEffect;
class TimeScale;

class Block : public CollisionCallBack, public ColliderParentObject
{
private:
	class Bomber
	{
		enum PHASE { NONE, EXPAND, WAIT, OCCUR }m_phase = NONE;
		Block* m_parent;
		Timer m_timer;
		Vec3<float>m_startScale;
		bool m_isStart;

	public:
		Bomber(Block* arg_parent) :m_parent(arg_parent) {}
		void Init() 
		{ 
			m_phase = NONE;
			m_isStart = false;
		}
		void Update(const TimeScale& arg_timeScale);

		void Explosion(Vec3<float>arg_startScale);
		const bool& IsStart()const { return m_isStart; }

	}m_explosion;

	void OnCollisionEnter(
		const CollisionResultInfo& arg_info,
		std::weak_ptr<Collider>arg_otherCollider)override {}

	void OnCollisionTrigger(
		const CollisionResultInfo& arg_info,
		std::weak_ptr<Collider>arg_otherCollider)override;

protected:
	static int s_hitSE;

	//�R���C�_�[
	std::shared_ptr<Collider>m_collider;
	//���������̃g�����X�t�H�[��
	Transform m_initTransform;
	//�@���ꂽ��
	int m_hitCount;

	//�q�b�g�G�t�F�N�g�|�C���^
	std::weak_ptr<TexHitEffect>m_hitEffect;

	//�����������ŌĂяo�����
	virtual void OnInit() = 0;
	//�X�V�����ŌĂяo�����
	virtual void OnUpdate() = 0;
	//�`�揈���ŌĂяo�����
	virtual void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam) = 0;

	//�@���ꂽ�u�ԌĂяo�����
	virtual void OnHitTrigger() = 0;

	//���S����
	virtual bool IsDead()const = 0;

	//�������o
	void Explosion() { m_explosion.Explosion(m_transform.GetScale()); }
	virtual void OnExplosionFinishTrigger() {};

public:
	static void StaticAwake(int arg_hitSE)
	{
		s_hitSE = arg_hitSE;
	}

	//�g�����X�t�H�[��
	Transform m_transform;
	enum TYPE { COIN, SLOT, NUM };

	Block(std::shared_ptr<Collider>arg_origin);

	virtual ~Block() {}
	//������
	void Init(Transform& arg_initTransform, const std::shared_ptr<TexHitEffect>& arg_hitEffect);
	//�X�V
	void Update(const TimeScale& arg_timeScale);
	//�`��
	void Draw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);

	//���ł�����
	virtual bool IsDisappear() = 0;

	//�u���b�N�̎�ʃQ�b�^
	virtual TYPE GetType()const = 0;

	//�R���C�_�[�Q�b�^
	std::shared_ptr<Collider>GetCollider() { return m_collider; }
};

class CoinBlock : public Block
{
	int m_hp;
	bool isExplosion = false;

	void OnInit()override { isExplosion = false; }
	void OnUpdate()override {}
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
	bool IsDead()const override
	{
		return m_hp <= m_hitCount;
	}

	void OnExplosionFinishTrigger()override { isExplosion = true; }
public:
	CoinBlock(std::shared_ptr<Collider>arg_origin, int arg_hp = 1);

	bool IsDisappear()override { return isExplosion; }

	TYPE GetType()const override { return COIN; }
};

class SlotBlock : public Block
{
	//�X���b�g�}�V���|�C���^
	std::weak_ptr<SlotMachine>m_slotMachinePtr;

	int m_hp;

	void OnInit()override;
	void OnUpdate()override;
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
	bool IsDead()const override
	{
		return m_hp <= m_hitCount;
	}
public:
	SlotBlock(std::shared_ptr<Collider>arg_origin, const std::shared_ptr<SlotMachine>& arg_slotMachine, int arg_hp = 3);

	bool IsDisappear()override { return IsDead(); }

	TYPE GetType()const override { return SLOT; }
};