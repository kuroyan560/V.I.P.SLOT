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
#include"Object.h"

void PlayersNormalAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
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
		m_hitEffect.lock()->Emit(arg_info.m_inter);
	}
}

void PlayersParryAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
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

void PlayersCounterAttack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
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
	m_hitEffect->Emit(arg_info.m_inter, m_hitCount);
}


void DamagedCallBack::Execute()
{
	using namespace ConstParameter::Player;

	//無敵時間中か
	if (!m_invincibleTimer.IsTimeUp())return;

	//攻撃中か
	if (m_parent->IsAttack())return;

	//HP減少
	bool consumeLife = m_parent->Damage(1);

	//無敵時間設定
	m_invincibleTimer.Reset(INVINCIBLE_TIME_ON_DAMAGED);

	//ヒットストップ
	m_hitStopTimer.Reset(consumeLife ? HIT_STOP_TIME_ON_DAMAGED_CONSUME_LIFE : HIT_STOP_TIME_ON_DAMAGED);

	//ヒットストップSE再生
	AudioApp::Instance()->PlayWave(m_hitStopSE);

	//点滅
	m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);
}

void DamagedCallBack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider>arg_otherCollider)
{
	this->Execute();
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

void PushBackCallBack::OnCollisionTrigger(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
{

}

void PushBackCallBack::OnCollisionEnter(const CollisionResultInfo& arg_info, std::weak_ptr<Collider> arg_otherCollider)
{
	//プレイヤーの座標取得
	auto pos = m_player->m_modelObj->m_transform.GetPos();
	//プレイヤーの座標の変化量取得
	Vec3<float>playerMove = pos - m_player->m_oldPos;

	//移動していない
	if (playerMove.Length() <= FLT_EPSILON)return;

	//足場化
	bool isScaffold = arg_otherCollider.lock()->HaveTag("Scaffold");

	//足場か
	if (isScaffold)
	{
		//足場との判定を切っている
		if (!m_notPushBackWithScaffoldTimer.IsTimeUp())return;
		//落下していない
		if (0.0f <= playerMove.y)return;
	}

	
	//移動ベクトルとの衝突点を求める
	CollisionResultInfo info;
	CollisionLine moveRay(arg_info.m_inter - playerMove, playerMove.GetNormal(), 20.0f);
	moveRay.HitCheckDispatch(XMMatrixIdentity(),
		arg_otherCollider.lock()->GetTransformMat(),
		arg_info.m_hitOtherPrimitive,
		&info);

	float radius = m_player->m_bodySphereCol->m_radius;
	auto offset = m_player->m_bodySphereCol->m_offset;
	auto aabb = (CollisionAABB*)arg_info.m_hitOtherPrimitive;

	//足場はY軸方向、落下時のみ押し戻し
	if (!isScaffold)
	{
		//X軸方向押し戻し
		if (playerMove.x && (m_player->m_oldPos.x < aabb->GetPtValue().x.m_min || aabb->GetPtValue().x.m_max < m_player->m_oldPos.x))pos.x = info.m_inter.x + radius * -(playerMove.GetNormal().x / abs(playerMove.GetNormal().x));
		//if (playerMove.x)pos.x = arg_info.m_inter.x + m_player->m_bodySphereCol->m_radius * -(playerMove.GetNormal().x / abs(playerMove.GetNormal().x));
		//Z軸方向押し戻し
		if (playerMove.z)pos.z = info.m_inter.z + radius * -(playerMove.GetNormal().z / abs(playerMove.GetNormal().z));
		//if (playerMove.z)pos.z = arg_info.m_inter.z + m_player->m_bodySphereCol->m_radius * -(playerMove.GetNormal().z / abs(playerMove.GetNormal().z));
	}
	//Y軸方向押し戻し
	if (playerMove.y)pos.y = info.m_inter.y + radius * -(playerMove.GetNormal().y / abs(playerMove.GetNormal().y));
	//if (playerMove.y)pos.y = arg_info.m_inter.y + m_player->m_bodySphereCol->m_radius * -(playerMove.GetNormal().y / abs(playerMove.GetNormal().y));
	
	pos += offset;

	m_player->OnLanding(!isScaffold);

	//押し戻し後の座標適用
	m_player->m_modelObj->m_transform.SetPos(pos);
}
