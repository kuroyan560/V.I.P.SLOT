#include "Block.h"
#include"SlotMachine.h"
#include"Collision.h"
#include"ConstParameters.h"
#include"Collider.h"
#include"TexHitEffect.h"
#include"TimeScale.h"
#include"Player.h"
#include"AudioApp.h"

int Block::s_hitSE = 0;

void Block::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
{
	//ヒットSE呼び出し
	AudioApp::Instance()->PlayWave(s_hitSE);

	//ヒットエフェクト
	m_hitEffect.lock()->Emit(arg_info.m_inter);

	//ヒット回数インクリメント
	m_hitCount++;

	//継承先の当たった瞬間の処理呼び出し
	OnHitTrigger();

	//ブロック破壊判定
	if (this->IsDead())
	{
		//爆発
		m_explosion.Explosion(m_transform.GetScale());
		//コライダーを切る
		m_collider->SetActive(false);
	}
}

void Block::Init(Transform& arg_initTransform, const std::shared_ptr<TexHitEffect>& arg_hitEffect)
{
	//初期化トランスフォームの記録と適用
	m_initTransform = arg_initTransform;
	m_initTransform.GetWorldMat();
	m_transform = m_initTransform;

	//叩かれた回数リセット
	m_hitCount = 0;

	//ヒットエフェクトアタッチ
	m_hitEffect = arg_hitEffect;

	OnInit();

	m_explosion.Init();
	m_collider->SetActive(true);
}

Block::Block(std::shared_ptr<Collider>arg_origin) :m_explosion(this)
{
	m_collider = std::make_shared<Collider>(arg_origin->Clone(&m_transform, this));
	m_collider->SetParentObject(this);
	m_collider->SetParentTransform(&m_transform);
	m_collider->SetCallBack("Player_Attack", this);
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
	this->Explosion();
}

CoinBlock::CoinBlock(std::shared_ptr<Collider>arg_origin, int arg_hp)
	:Block(arg_origin), m_hp(arg_hp)
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
		m_slotMachinePtr.lock()->Booking();
	}
}

SlotBlock::SlotBlock(std::shared_ptr<Collider>arg_origin, const std::shared_ptr<SlotMachine>& arg_slotMachine, int arg_hp)
	:Block(arg_origin), m_slotMachinePtr(arg_slotMachine), m_hp(arg_hp)
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
	if (m_isStart)return;

	static const float EXPAND_TIME = 10.0f;
	m_phase = EXPAND;
	m_startScale = arg_startScale;
	m_timer.Reset(EXPAND_TIME);
	m_isStart = true;
}
