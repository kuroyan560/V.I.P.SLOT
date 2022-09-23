#include "Enemy.h"
#include"EnemyBreed.h"
#include"ActPoint.h"
#include"Player.h"
#include"ColliderWithFloor.h"
#include"HitEffect.h"

void Enemy::Init()
{
	m_hp = m_breed.GetMaxHp();
	if (m_control)m_control->Init();

	m_withFloorCollider->Init();
}

void Enemy::Update(const Player& Player, const float& Gravity)
{
	//死んでいる
	if (!IsAlive())return;

	//移動量
	Vec3<float>move = { 0,0,0 };

	//自身の座標
	Vec3<float>myPos = m_transform.GetPos();

	//プレイヤーのラッシュシステム取得
	const auto& rushSys = Player.GetRushSys();

	bool onLock = false;
	bool onRush = false;
	//ActPointの通知を受け取ってラッシュされた瞬間ならラッシュ巻き込まれフラグON
	for (const auto& actPt : m_actPointArray)
	{
		if (!onRush)onRush = actPt->OnRushBlow();
		if (!onLock)onLock = actPt->OnLock();
	}

	//ラッシュに巻き込まれているか判定
	//※Lastラッシュフラグと同時にONになっている可能性があるので、下のif文(m_involvedRush)と繋げない
	if (!m_involvedRush && onRush)
	{
		//ふっ飛ばされる直前の座標
		m_beforeBlowPos = myPos;
		//
		m_withFloorCollider->Jump(rushSys.GetBlowPowerY());

		//ダメージ
		Damage(rushSys.GetOneRushDamage());

		//エフェクト
		HitEffect::Generate(myPos);

		m_involvedRush = true;
	}

	//ラッシュに巻き込まれて動けなくなる
	if (!m_involvedLocked && onLock)m_involvedLocked = true;

	//ラッシュが終了しているか
	if (m_involvedRush && rushSys.IsFinishRush())
	{
		//巻き込まれフラグoffに
		m_involvedRush = false;
		//再び動けるようになる
		m_involvedLocked = false;
	}

	//ラッシュに巻き込まれている最中
	if (m_involvedRush)
	{
		//１ラッシュが終わったタイミング
		if (rushSys.IsOneRushTrigger())
		{
			//ふっ飛ばされる直前の座標
			m_beforeBlowPos = myPos;
			//ダメージ
			Damage(rushSys.GetOneRushDamage());
			//エフェクト
			HitEffect::Generate(myPos);
		}

		//最後のラッシュ中か
		if (rushSys.IsLastRush())
		{
			//プレイヤーが向いている方向のふっ飛ばされる
			move += Player.GetTransform().GetFront() * rushSys.GetNowBlowPower();
		}
		else
		{
			//巻き込まれてふっとばされた先の座標
			Vec3<float>involvedTargetPos = rushSys.GetInvolvedObjsTargetPos();
			//ふっ飛ばしの時間割合
			float blowRate = rushSys.GetBlowRate();

			//ふっ飛ばされ先の座標に近づく
			//m_transform.SetPos(KuroMath::Lerp(m_transform.GetPos(), involvedTargetPos, 0.1f));
			m_transform.SetPos(KuroMath::Ease(Out, Cubic, blowRate, m_beforeBlowPos, involvedTargetPos));
		}
	}

	//挙動が設定されていて、ラッシュによって動きがロックされていなければ、その更新処理
	if (m_control && !m_involvedLocked)m_control->Update();

	//落下
	move.y += m_withFloorCollider->GetFallSpeed();
	m_withFloorCollider->AddFallSpeed(Gravity * (m_involvedRush ? rushSys.GetBlowGravityRate() : 1.0f));

	//移動量を反映させる
	auto newPos = myPos;
	newPos += move;
	m_transform.SetPos(newPos);
}

void Enemy::Damage(const int& Amount)
{
	m_hp -= Amount;
}