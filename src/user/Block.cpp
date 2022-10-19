#include "Block.h"
#include"SlotMachine.h"
#include"Collision.h"
#include"ConstParameters.h"
#include"Collider.h"
#include"TexHitEffect.h"
#include"TimeScale.h"

void Block::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	m_hitEffect.lock()->Emit(arg_inter);
	m_hitCount++;
	OnHitTrigger();

	if (this->IsDead())
	{

	}
}

void Block::Init(Transform& arg_initTransform, std::shared_ptr<Collider>& arg_attachCollider, const std::shared_ptr<TexHitEffect>& arg_hitEffect)
{
	//初期化トランスフォームの記録と適用
	m_initTransform = arg_initTransform;
	m_initTransform.GetMat();
	m_transform = m_initTransform;

	//叩かれた回数リセット
	m_hitCount = 0;

	//アタッチされたコライダーを記録
	m_attachCollider = arg_attachCollider;
	arg_attachCollider->SetParentObject(this);
	arg_attachCollider->SetParentTransform(&m_transform);
	arg_attachCollider->SetCallBack("Player", this);
	arg_attachCollider->SetActive(true);

	//ヒットエフェクトアタッチ
	m_hitEffect = arg_hitEffect;

	OnInit();

	m_explosion.Init();
}

void Block::Update(const TimeScale& arg_timeScale)
{
	OnUpdate();
	m_explosion.Update(arg_timeScale);
}

void Block::Draw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
	OnDraw(arg_transform, arg_lightMgr, arg_cam);
}

void CoinBlock::OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
}

void CoinBlock::OnHitTrigger()
{
	m_attachCollider.lock()->SetActive(false);
	this->Explosion();
}

CoinBlock::CoinBlock(int arg_hp)
	:m_hp(arg_hp)
{

}

void SlotBlock::OnInit()
{
}

void SlotBlock::OnUpdate()
{
}

void SlotBlock::OnDraw(Transform& arg_transform, std::weak_ptr<LightManager>& arg_lightMgr, std::weak_ptr<Camera>& arg_cam)
{
}

void SlotBlock::OnHitTrigger()
{
	if (IsDead())
	{
		m_slotMachinePtr.lock()->Lever();
		m_attachCollider.lock()->SetActive(false);
	}
}

SlotBlock::SlotBlock(const std::shared_ptr<SlotMachine>& arg_slotMachine, int arg_hp)
	:m_slotMachinePtr(arg_slotMachine), m_hp(arg_hp)
{
}

void Block::Bomber::Update(const TimeScale& arg_timeScale)
{
	if (m_phase == NONE)return;

	bool isTimeUp = m_timer.UpdateTimer(arg_timeScale.GetTimeScale());

	static const float EXPAND_SCALE = 1.2f;
	static const float WAIT_TIME = 10.0f;
	static const float OCCUR_SCALE = 2.0f;
	static const float OCCUR_TIME = 10.0f;

	switch (m_phase)
	{
	case EXPAND:
	{
		m_parent->m_transform.SetScale(
			KuroMath::Ease(Out, Exp, m_timer.GetTimeRate(), m_startScale, m_startScale * EXPAND_SCALE));

		if (isTimeUp)
		{
			m_phase = WAIT;
			m_timer.Reset(WAIT_TIME);
		}
		break;
	}
	case WAIT:
	{
		if (isTimeUp)m_timer.Reset(OCCUR_TIME);
	}
	case OCCUR:
	{
		m_parent->m_transform.SetScale(
			KuroMath::Ease(In, Exp, m_timer.GetTimeRate(), m_startScale * EXPAND_SCALE, m_startScale * OCCUR_SCALE));

		//爆発した瞬間の関数呼び出し
		if (isTimeUp)
		{
			m_parent->OnExplosionFinishTrigger();
			m_phase = NONE;
			m_parent->m_transform.SetScale(m_startScale);
		}
		break;
	}
	}
}

void Block::Bomber::Explosion(Vec3<float> arg_startScale)
{
	static const float EXPAND_TIME = 10.0f;
	m_phase = EXPAND;
	m_startScale = arg_startScale;
	m_timer.Reset(EXPAND_TIME);
}
