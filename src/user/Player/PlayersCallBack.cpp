#include "PlayersCallBack.h"
#include"Collider.h"
#include"GameObject.h"
#include"TexHitEffect.h"
#include"AudioApp.h"
#include"ObjectManager.h"
#include"Player.h"
#include"Yoyo.h"
#include"TimeScale.h"
#include"GameCamera.h"
#include"CoinVault.h"

void PlayersNormalAttack::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	//コライダーの親に設定されているオブジェクトのポインタ取得
	auto obj = arg_otherCollider.lock()->GetParentObject<GameObject>();
	//オブジェクトにダメージを与える
	if (int coinNum = obj->Damage(*m_offensive))
	{
		//死亡SE
		AudioApp::Instance()->PlayWave(m_killSE);
		//プレイヤーにお金を渡す
		m_playersVault->Add(coinNum);
	}
	else
	{
		//ダメージSE
		AudioApp::Instance()->PlayWave(m_hitSE);
		//ヒットエフェクト
		m_hitEffect.lock()->Emit(arg_inter);
	}
}

void PlayersParryAttack::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	//コライダーの親に設定されているオブジェクトのポインタ取得
	auto obj = arg_otherCollider.lock()->GetParentObject<GameObject>();
	//オブジェクトにダメージを与える
	obj->Damage(*m_offensive);
	//敵の攻撃の親オブジェクト取得
	auto parentObj = obj->GetParentObj();
	//親オブジェクトに向かって弾発射
	if (parentObj)
	{
		//パリィ弾の発射
		m_objMgr.lock()->AppearParryBullet(
			m_colMgr,
			obj->m_transform.GetPos(),
			parentObj);
	}
	//SE
	AudioApp::Instance()->PlayWave(m_parrySE);
}

void PlayersCounterAttack::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	//コライダーの親に設定されているオブジェクトのポインタ取得
	auto obj = arg_otherCollider.lock()->GetParentObject<GameObject>();

	//ヒットカウントインクリメント
	m_hitCount++;

	//オブジェクトにダメージを与える
	if (int coinNum = obj->Damage(*m_offensive))
	{
		//死亡SE
		AudioApp::Instance()->PlayWave(m_killSE);
		//プレイヤーにお金を渡す（当てた数が増えるほど得られるコインが増える）
		m_playersVault->Add(coinNum * m_hitCount);
	}
	else
	{
		//ダメージSE
		AudioApp::Instance()->PlayWave(m_hitSE);
	}

	//ヒットエフェクト
	m_hitEffect->Emit(arg_inter, m_hitCount);
}


void DamagedCallBack::OnCollisionTrigger(const Vec3<float>& arg_inter,
	std::weak_ptr<Collider>arg_otherCollider)
{
	using namespace ConstParameter::Player;

	//無敵時間中か
	if (!m_invincibleTimer.IsTimeUp())return;

	//攻撃中か
	if (m_parent->IsAttack())return;

	//HP減少
	m_parent->Damage(1);

	//無敵時間設定
	m_invincibleTimer.Reset(INVINCIBLE_TIME_ON_DAMAGED);

	//ヒットストップ
	m_hitStopTimer.Reset(HIT_STOP_TIME_ON_DAMAGED);

	//ヒットストップSE再生
	AudioApp::Instance()->PlayWave(m_hitStopSE);

	//点滅
	m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);
}

void DamagedCallBack::Update(TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	const float& timeScale = arg_timeScale.GetTimeScale();

	//ヒットストップ始め
	if (m_hitStopTimer.IsTimeStartOnTrigger())
	{
		arg_timeScale.Set(0.0f);
	}
	//ヒットストップ終わり
	else if (m_hitStopTimer.IsTimeUpOnTrigger())
	{
		arg_timeScale.Set(1.0f);
		AudioApp::Instance()->PlayWave(m_damageSE);
		//カメラ振動
		m_cam.lock()->Shake(60, 2, 2.0f, 1.0f);
	}

	m_hitStopTimer.UpdateTimer();

	//無敵時間
	if (m_invincibleTimer.UpdateTimer(timeScale))
	{
		//点滅終了
		m_isDraw = true;
	}
	else
	{
		//点滅
		if (m_flashTimer.UpdateTimer(timeScale))
		{
			m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);
			m_isDraw = !m_isDraw;
		}
	}
}

