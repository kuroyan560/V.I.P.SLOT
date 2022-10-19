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
		bool m_finish;

	public:
		Bomber(Block* arg_parent) :m_parent(arg_parent) {}
		void Init() 
		{ 
			m_phase = NONE;
			m_finish = false;
		}
		void Update(const TimeScale& arg_timeScale);

		void Explosion(Vec3<float>arg_startScale);
		const bool& IsFinish()const { return m_finish; }

	}m_explosion;

	void OnCollisionEnter(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override {}

	void OnCollisionTrigger(
		const Vec3<float>& arg_inter,
		std::weak_ptr<Collider>arg_otherCollider)override;

protected:
	//コライダー
	std::weak_ptr<Collider>m_attachCollider;
	//初期化時のトランスフォーム
	Transform m_initTransform;
	//叩かれた回数
	int m_hitCount;

	//ヒットエフェクトポインタ
	std::weak_ptr<TexHitEffect>m_hitEffect;

	//初期化処理で呼び出される
	virtual void OnInit() = 0;
	//更新処理で呼び出される
	virtual void OnUpdate() = 0;
	//描画処理で呼び出される
	virtual void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam) = 0;

	//叩かれた瞬間呼び出される
	virtual void OnHitTrigger() = 0;

	//死亡判定
	virtual bool IsDead() = 0;

	//爆発演出
	void Explosion() { m_explosion.Explosion(m_transform.GetScale()); }
	virtual void OnExplosionFinishTrigger() {};

public:
	//トランスフォーム
	Transform m_transform;
	enum TYPE { COIN, SLOT, NUM };

	Block() :m_explosion(this)
	{
	}

	virtual ~Block() {}
	//初期化
	void Init(Transform& arg_initTransform, std::shared_ptr<Collider>& arg_attachCollider, const std::shared_ptr<TexHitEffect>& arg_hitEffect);
	//更新
	void Update(const TimeScale& arg_timeScale);
	//描画
	void Draw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam);

	//消滅したか
	bool IsDisappear() { return m_explosion.IsFinish(); }

	virtual TYPE GetType() = 0;

	//コライダーゲッタ
	std::shared_ptr<Collider>GetCollider() { return m_attachCollider.lock(); }
};

class CoinBlock : public Block
{
	int m_hp;
	bool isExplosion = false;

	void OnInit()override { isExplosion = false; }
	void OnUpdate()override {}
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
	bool IsDead()override
	{
		return m_hp <= m_hitCount;
	}

	
	void OnExplosionFinishTrigger()override { isExplosion = true; }
public:
	CoinBlock(int arg_hp = 1);

	TYPE GetType()override { return COIN; }
};

class SlotBlock : public Block
{
	//スロットマシンポインタ
	std::weak_ptr<SlotMachine>m_slotMachinePtr;

	int m_hp;

	void OnInit()override;
	void OnUpdate()override;
	void OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)override;
	void OnHitTrigger()override;
	bool IsDead()override
	{
		return m_hp <= m_hitCount;
	}
public:
	SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine, int arg_hp = 3);

	TYPE GetType()override { return SLOT; }

};