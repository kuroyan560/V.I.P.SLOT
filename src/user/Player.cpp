#include "Player.h"
#include"Object.h"
#include"UsersInput.h"
#include"ConstParameters.h"
#include"AudioApp.h"
#include"SlotMachine.h"
#include"TimeScale.h"
#include"Collision.h"
#include"Collider.h"
#include"CollisionManager.h"
#include"GameCamera.h"

void Player::Jump()
{
	m_move.y = 0.0f;
	m_fallSpeed = ConstParameter::Player::JUMP_POWER;
}

Player::Player(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Player;

	//モデル読み込み
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	//ジャンプSE読み込み
	m_jumpSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_jump.wav",0.6f);

	//BETのSE読み込み
	m_betSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav",0.15f);

	//被ダメージヒットストップSE
	int onDamagedHitStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_hitStop.wav",0.5f);

	//被ダメージSE
	int onDamagedSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_onTrigger.wav",0.4f);

	//敵の踏みつけSE
	int onStepEnemySE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_step.wav",0.8f);

	/*--- コライダー用プリミティブ生成 ---*/

	//モデル全体を覆う球
	std::shared_ptr<CollisionPrimitive>bodySphereCol = std::make_shared<CollisionSphere>(
		1.2f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -0.2f, 0.0f),
		&m_modelObj->m_transform);

	//足元の当たり判定球
	std::shared_ptr<CollisionPrimitive>footSphereCol = std::make_shared<CollisionSphere>(
		1.0f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -1.5f, 0.0f),
		&m_modelObj->m_transform);

	/*--- コールバック生成 ---*/
	//被ダメージコールバック
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, onDamagedHitStopSE, onDamagedSE);
	//攻撃コールバック
	m_attackCallBack = std::make_shared<AttackCallBack>(this, onStepEnemySE);

	/*--- コライダー生成 ---*/

	//モデル全体を覆うコライダー
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			bodySphereCol
		};
		std::shared_ptr<Collider>bodyCollider = std::make_shared<Collider>("Player_Body", coverModelPrimitiveArray);

		//被ダメージコールバックアタッチ
		bodyCollider->SetCallBack(m_damegedCallBack, arg_collisionMgr.lock()->GetAttribute("Enemy"));
		m_colliders.emplace_back(bodyCollider);
	}
	
	//足元のコライダー
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>footPrimitiveArray = { footSphereCol };
		std::shared_ptr<Collider>footCollider = std::make_shared<Collider>("Player_Foot", footPrimitiveArray);

		//攻撃コールバックアタッチ
		footCollider->SetCallBack(m_attackCallBack, arg_collisionMgr.lock()->GetAttribute("Enemy"));
		m_colliders.emplace_back(footCollider);
	}


	/*--- コライダー配列登録 ---*/
	arg_collisionMgr.lock()->Register("Player", m_colliders);
}

void Player::Init(std::weak_ptr<GameCamera>arg_cam)
{
	using namespace ConstParameter::Player;

	//HP初期化
	m_hp = MAX_HP;

	//スタート位置に移動
	m_modelObj->m_transform.SetPos(INIT_POS);

	//移動速度
	m_move = { 0,0,0 };

	//落下速度初期化
	m_fallSpeed = 0.0f;

	//接地フラグ初期化
	m_isOnGround = true;

	//所持金リセット
	m_coinVault.Set(300000);

	//BETのスパン計測用タイマー
	m_betTimer.Reset(0);

	//連続BETの計測用タイマー
	m_consecutiveBetTimer.Reset(UNTIL_MAX_SPEED_BET_TIME);

	//被ダメージコールバック
	m_damegedCallBack->Init(arg_cam);
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale)
{
//入力情報取得
	const auto& input = *UsersInput::Instance();

	//左スティック（移動）入力
	const auto stickVec = input.GetLeftStickVec(0);

	//Aボタン（ジャンプ）入力
	const bool jumpTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::A);

	//Xボタン（前方にショット）入力
	const bool shotTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::X);

	//LBボタン（BET）入力
	const bool betInput = input.ControllerInput(0, XBOX_BUTTON::LB);

//入力情報を元に操作
	//横移動
	if (0.0f < stickVec.x)
	{
		m_move.x = KuroMath::Lerp(m_move.x, ConstParameter::Player::MOVE_SPEED, ConstParameter::Player::MOVE_LERP_RATE);
	}
	else if (stickVec.x < 0.0f)
	{
		m_move.x = KuroMath::Lerp(m_move.x, -ConstParameter::Player::MOVE_SPEED, ConstParameter::Player::MOVE_LERP_RATE);
	}
	else
	{
		m_move.x = KuroMath::Lerp(m_move.x, 0.0f, ConstParameter::Player::MOVE_LERP_RATE);
	}

	//ジャンプ
	if (jumpTrigger && m_isOnGround)
	{
		Jump();
		AudioApp::Instance()->PlayWaveDelay(m_jumpSE,3);
		m_isOnGround = false;
	}

	//落下（ジャンプ中と落下中で重力変化、素早くジャンプ → ゆっくり降下）
	m_move.y += m_fallSpeed * arg_timeScale.GetTimeScale();
	if (0.0f < m_fallSpeed)
	{
		m_fallSpeed -= ConstParameter::Player::STRONG_GRAVITY * arg_timeScale.GetTimeScale();
	}
	else
	{
		m_fallSpeed -= ConstParameter::Player::WEAK_GRAVITY * arg_timeScale.GetTimeScale();
	}

	//落下速度加減
	if (m_fallSpeed < ConstParameter::Player::FALL_SPEED_MIN)m_fallSpeed = ConstParameter::Player::FALL_SPEED_MIN;

	//移動量加算
	auto pos = m_modelObj->m_transform.GetPos();
	pos += m_move * arg_timeScale.GetTimeScale();

	//押し戻し（床）
	if (pos.y < 0.0f)
	{
		pos.y = 0.0f;
		m_fallSpeed = 0.0f;
		m_move.y = 0.0f;
		m_isOnGround = true;
	}

	//押し戻し（ステージ端）
	if (pos.x < -ConstParameter::Environment::FIELD_WIDTH_HALF)
	{
		pos.x = -ConstParameter::Environment::FIELD_WIDTH_HALF;
		m_move.x = 0.0f;
	}
	else if(ConstParameter::Environment::FIELD_WIDTH_HALF < pos.x)
	{
		pos.x = ConstParameter::Environment::FIELD_WIDTH_HALF;
		m_move.x = 0.0f;
	}

	//更新した座標の反映
	m_modelObj->m_transform.SetPos(pos);

	//コインのBET
	if (betInput)
	{
		//コイン投入
		if (m_betTimer.UpdateTimer(arg_timeScale.GetTimeScale()))
		{
			//スロットマシンにBET
			arg_slotMachine.lock()->Bet(ConstParameter::Player::PASS_COIN_NUM, m_modelObj->m_transform);

			//BETスパン計算
			int betSpan = KuroMath::Lerp(ConstParameter::Player::BET_SPEED_MIN_SPAN, ConstParameter::Player::BET_SPEED_MAX_SPAN,
				m_consecutiveBetTimer.GetTimeRate());

			//次にBETするまでの時間
			m_betTimer.Reset(betSpan);

			//BETのSE再生
			AudioApp::Instance()->PlayWave(m_betSE);
		}

		//連続BETの時間計測
		m_consecutiveBetTimer.UpdateTimer(arg_timeScale.GetTimeScale());
	}
	//次の入力トリガー時は即コイン投入
	else
	{
		m_betTimer.Reset(0);
		m_consecutiveBetTimer.Reset();
	}

	//被ダメージコールバック
	m_damegedCallBack->Update(arg_timeScale);
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (!m_damegedCallBack->GetIsDraw())return;

	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
}

void Player::DamagedCallBack::OnCollision(const Vec3<float>& arg_inter, 
	std::weak_ptr<Collider>arg_otherCollider,
	const unsigned char& arg_otherAttribute, 
	const CollisionManager& arg_collisionMgr)
{
	using namespace ConstParameter::Player;

	//無敵時間中か
	if (!m_invincibleTimer.IsTimeUp())return;

	//HP減少
	m_parent->m_hp--;

	//無敵時間設定
	m_invincibleTimer.Reset(INVINCIBLE_TIME_ON_DAMAGED);

	//ヒットストップ
	m_hitStopTimer.Reset(HIT_STOP_TIME_ON_DAMAGED);

	//ヒットストップSE再生
	AudioApp::Instance()->PlayWave(m_hitStopSE);

	//点滅
	m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);

	//カメラ振動
	m_cam.lock()->Shake(60, 2, 2.0f, 1.0f);

	printf("Player : Damaged : remain hp %d\n", m_parent->m_hp);
}

void Player::DamagedCallBack::Update(TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;

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
	}

	m_hitStopTimer.UpdateTimer();

	//無敵時間
	if (m_invincibleTimer.UpdateTimer(arg_timeScale.GetTimeScale()))
	{
		//点滅終了
		m_isDraw = true;
	}
	else
	{
		//点滅
		if (m_flashTimer.UpdateTimer(arg_timeScale.GetTimeScale()))
		{
			m_flashTimer.Reset(FLASH_SPAN_ON_DAMAGED_INVINCIBLE);
			m_isDraw = !m_isDraw;
		}
	}
}

#include"Enemy.h"
void Player::AttackCallBack::OnCollision(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider, const unsigned char& arg_otherAttribute, const CollisionManager& arg_collisionMgr)
{
	using namespace ConstParameter::Player;

	//プレイヤーより衝突点が上（踏みつけできない）
	if (m_parent->m_modelObj->m_transform.GetPos().y + FIX_MODEL_CENTER_OFFSET.y < arg_inter.y)return;

	auto enemy = arg_otherCollider.lock()->GetParentObject<Enemy>();
	enemy->Damage();
	m_parent->Jump();
	AudioApp::Instance()->PlayWave(m_stepEnemySE);
}
