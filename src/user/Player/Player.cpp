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
#include"Block.h"
#include"YoYo.h"
#include"Scaffold.h"
#include"TexHitEffect.h"
#include"BasicDraw.h"
#include"PlayersCounterAttackHitEffect.h"

void Player::OnAir()
{
	m_isOnGround = false;
	m_isOnScaffold = false;

	//敵踏みつけコライダーオン
	m_footSphereCollider->SetActive(true);
}

void Player::Jump()
{
	m_move.y = 0.0f;
	m_fallSpeed = ConstParameter::Player::JUMP_POWER;
	OnAir();
}

void Player::OnLanding(bool arg_isGround)
{
	m_fallSpeed = 0.0f;
	m_move.y = 0.0f;
	if (arg_isGround)m_isOnGround = true;
	else m_isOnScaffold = true;

	//敵踏みつけコライダーオフ
	m_footSphereCollider->SetActive(false);
}

void Player::OnImguiItems()
{
	ImGui::Text("Coin : { %d }", m_coinVault.GetNum());
	ImGui::Text("Offensive : { %d }", m_ability.m_offensive);

	m_drawParam.ImguiDebugItem();
	m_yoYo->AddImguiDebugItem();
}

void Player::InitMovement()
{
	using namespace ConstParameter::Player;

	//スタート位置に移動
	m_modelObj->m_transform.SetPos(INIT_POS);

	m_oldPos = INIT_POS;

	//移動速度
	m_move = { 0,0,0 };

	//落下速度初期化
	m_fallSpeed = 0.0f;

	//接地フラグ初期化
	m_isOnGround = true;

	//ヨーヨー
	m_yoYo->Init();

	//デフォルト右向き
	m_vecX = 1.0f;

	//足場から降りる処理初期化
	m_stepDown = false;
}

Player::Player() : Debugger("Player")
{
	//モデル読み込み
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	std::string soundDir = "resource/user/sound/";

	//ジャンプSE読み込み
	m_jumpSE = AudioApp::Instance()->LoadAudio(soundDir + "player_jump.wav", 0.5f);

	//敵への攻撃SE
	int enemyHitSE = AudioApp::Instance()->LoadAudio(soundDir + "enemy_damage.wav", 0.5f);

	//敵を倒したときのSE
	int enemyKillSE = AudioApp::Instance()->LoadAudio(soundDir + "enemy_dead.wav", 0.5f);

	//ヒットエフェクト生成
	m_hitEffect = std::make_shared<TexHitEffect>();
	m_hitEffect->Set("resource/user/img/hitEffect.png", 5, { 5,1 }, { 6.0f,6.0f }, 3, false);
	m_counterHitEffect = std::make_shared<PlayersCounterAttackHitEffect>();

	//攻撃コールバック
	m_normalAttackCallBack = std::make_shared<PlayersNormalAttack>(&m_ability.m_offensive,&m_coinVault, m_hitEffect, enemyHitSE, enemyKillSE);
	//反射攻撃コールバック
	m_counterAttackCallBack = std::make_shared<PlayersCounterAttack>(&m_ability.m_offensive, &m_coinVault, m_counterHitEffect.get(), enemyHitSE, enemyKillSE);
}

void Player::Awake(std::weak_ptr<CollisionManager> arg_collisionMgr, std::weak_ptr<ObjectManager> arg_objMgr, std::weak_ptr<GameCamera> arg_cam)
{
	using namespace ConstParameter::Player;

	std::string soundDir = "resource/user/sound/";

	//被ダメージヒットストップSE
	int onDamagedHitStopSE = AudioApp::Instance()->LoadAudio(soundDir + "player_damage_hitStop.wav", 0.5f);

	//被ダメージSE
	int onDamagedSE = AudioApp::Instance()->LoadAudio(soundDir + "player_damage_onTrigger.wav", 0.4f);

	//パリーSE
	int parrySE = AudioApp::Instance()->LoadAudio(soundDir + "parry.wav", 0.8f);

	/*--- コライダー用プリミティブ生成 ---*/
	//モデル全体を覆う球
	m_bodySphereColPrim = std::make_shared<CollisionSphere>(
		1.4f,
		Vec3<float>(0.0f, -0.2f, 0.0f));

	//モデル全体を覆うAABB
	m_bodyAABBColPrim = std::make_shared<CollisionAABB>(
		m_modelObj->m_model->GetAllMeshPosMinMax());

	//足元の当たり判定球
	m_footSphereColPrim = std::make_shared<CollisionSphere>(
		1.0f,
		Vec3<float>(0.0f, -1.5f, 0.0f));

	/*--- コールバック生成 ---*/
	//パリィ攻撃コールバック
	m_parryAttackCallBack = std::make_shared<PlayersParryAttack>(&m_ability.m_offensive, arg_objMgr, arg_collisionMgr, parrySE);
	//被ダメージコールバック
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, arg_cam, onDamagedHitStopSE, onDamagedSE);
	//押し戻し
	m_pushBackCallBack = std::make_shared<PushBackCallBack>(this);
	//回復キット回収
	m_getHealKitCallBack = std::make_shared<GetHealKitCallBack>(this);
	//踏みつけジャンプ
	m_stepCallBack = std::make_shared<StepCallBack>(this);

	/*--- コライダー生成（判定順） ---*/

	std::vector<std::shared_ptr<Collider>>colliders;

	//モデル全体を覆う球コライダー
	{
		m_bodySphereCollider = std::make_shared<Collider>();
		m_bodySphereCollider->Generate("Player_Body_Sphere", { "Player" }, { m_bodySphereColPrim });
		m_bodySphereCollider->SetParentObject(this);
		m_bodySphereCollider->SetParentTransform(&m_modelObj->m_transform);

		//被ダメージコールバックアタッチ
		m_bodySphereCollider->SetCallBack("Enemy", m_damegedCallBack.get());
		m_bodySphereCollider->SetCallBack("Enemy_Attack", m_damegedCallBack.get());

		//回復キット回収コールバックアタッチ
		m_bodySphereCollider->SetCallBack("Heal_Kit", m_getHealKitCallBack.get());

		colliders.emplace_back(m_bodySphereCollider);
	}
	//モデル全体を覆うAABBコライダー
	{
		m_bodyAABBCollider = std::make_shared<Collider>();
		m_bodyAABBCollider->Generate("Player_Body_AABB", { "Player" }, { m_bodyAABBColPrim });
		m_bodyAABBCollider->SetParentObject(this);
		m_bodyAABBCollider->SetParentTransform(&m_modelObj->m_transform);

		//押し戻しコールバックアタッチ
		m_bodyAABBCollider->SetCallBack("Scaffold", m_pushBackCallBack.get());
		m_bodyAABBCollider->SetCallBack("Floor", m_pushBackCallBack.get());

		colliders.emplace_back(m_bodyAABBCollider);
	}
	//モデルの足元の球コライダー
	{
		m_footSphereCollider = std::make_shared<Collider>();
		m_footSphereCollider->Generate("Player_Foot_Sphere", { "Player" }, { m_footSphereColPrim });
		m_footSphereCollider->SetParentObject(this);
		m_footSphereCollider->SetParentTransform(&m_modelObj->m_transform);

		//踏みつけジャンプコールバック
		m_footSphereCollider->SetCallBack("Enemy", m_stepCallBack.get());

		colliders.emplace_back(m_footSphereCollider);
	}

	/*--- コライダー配列登録 ---*/
	arg_collisionMgr.lock()->Register(colliders);

	/*--- ヨーヨー生成 ---*/
	m_yoYo = std::make_shared<YoYo>(
		arg_collisionMgr,
		&m_modelObj->m_transform,
		m_normalAttackCallBack,
		m_parryAttackCallBack);
	m_yoYo->Awake(3.0f, 2.5f);
}

void Player::Init(PlayersAbility arg_ability, int arg_initRemainLife, int arg_initCoinNum)
{
	using namespace ConstParameter::Player;

	//現在の能力値を更新
	m_ability = arg_ability;

	//HP初期化
	m_playerHp.Init(m_ability.m_maxLife, arg_initRemainLife);

	//所持金リセット
	m_coinVault.Set(arg_initCoinNum);

	//所持金リセット
	m_coinVault.Set(0);

	//被ダメージコールバック
	m_damegedCallBack->Init();

	InitMovement();

	//ヒットエフェクト
	m_hitEffect->Init();
	m_counterHitEffect->Init();

	//所持金コインUI
	m_coinUI.Init(m_coinVault.GetNum());
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	using namespace ConstParameter::Environment;

	//タイムスケール取得
	const float& timeScale = arg_timeScale.GetTimeScale();

	//座標取得
	Vec3<float>pos = m_modelObj->m_transform.GetPos();
	m_oldPos = pos;

//入力情報取得
	const auto& input = *UsersInput::Instance();

	//移動方向入力
	Vec2<float>moveInput = { 0,0 };

	//ジャンプのトリガー入力
	bool jumpTrigger = false;

	//ヨーヨー投げ入力
	bool throwYoyoTrigger = false;

	//N攻撃入力
	bool neutralTrigger = false;

	//入力設定
	switch (m_inputConfig)
	{
	//キーボード入力
	case Player::INPUT_CONFIG::KEY_BOARD:
	{
		//方向入力
		if (input.KeyInput(DIK_LEFT))moveInput.x = -1.0f;
		else if (input.KeyInput(DIK_RIGHT))moveInput.x = 1.0f;
		//ジャンプ入力
		jumpTrigger = input.KeyOnTrigger(DIK_UP);
		//ヨーヨー投げ入力
		throwYoyoTrigger = input.KeyOnTrigger(DIK_SPACE);
		//N攻撃入力
		neutralTrigger = input.KeyOnTrigger(DIK_LSHIFT);
		break;
	}

	//コントローラー入力
	case Player::INPUT_CONFIG::CONTROLLER:
	{
		//方向入力
		moveInput = input.GetLeftStickVec(0);
		//ジャンプ入力
		jumpTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::A);
		//ヨーヨー投げ入力
		throwYoyoTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::X);
		//N攻撃入力
		neutralTrigger = input.ControllerOnTrigger(0, XBOX_BUTTON::RB);
		break;
	}

	//その他の入力設定になっていた場合エラー
	default:
		assert(0);
		break;
	}

	//向き記録
	if (!moveInput.IsZero())m_vecX = moveInput.x;

//入力情報を元に操作
	float moveSpeed = m_yoYo->IsThrow() ? MOVE_SPEED_WHILE_THROW : MOVE_SPEED;

	//横移動
	if (0.0f < moveInput.x)
	{
		m_move.x = KuroMath::Lerp(m_move.x, moveSpeed, MOVE_LERP_RATE);
	}
	else if (moveInput.x < 0.0f)
	{
		m_move.x = KuroMath::Lerp(m_move.x, -moveSpeed, MOVE_LERP_RATE);
	}
	else
	{
		m_move.x = KuroMath::Lerp(m_move.x, 0.0f, MOVE_LERP_RATE);
	}

	//ジャンプ
	bool canJump = (m_isOnGround || m_isOnScaffold) && !m_yoYo->IsThrow();
	if (jumpTrigger && canJump)
	{
		Jump();
		AudioApp::Instance()->PlayWaveDelay(m_jumpSE, 3);
	}

	//足場との当たり判定を切る時間更新
	m_pushBackCallBack->Update(timeScale);
	//足場から降りる
	bool stepDownInput = (-moveInput.y < 0.0f);
	if (stepDownInput)
	{
		if (!m_stepDown && m_isOnScaffold)
		{
			m_pushBackCallBack->NotPushBackWithScaffold(3.0f);
			m_stepDown = true;
			m_fallSpeed = m_stepDownFallSpeed;
			OnAir();
		}
	}
	//入力をし直さないと足場降りを実行しない
	else
	{
		m_stepDown = false;
	}
	
	//攻撃勢い
	m_move.x += m_yoYo->GetAccelX() * timeScale;
	m_fallSpeed += m_yoYo->GetAccelY() * timeScale;

	//落下（ジャンプ中と落下中で重力変化、素早くジャンプ → ゆっくり降下）
	m_move.y += m_fallSpeed * timeScale;

	if (0.0f < m_fallSpeed)
	{
		m_fallSpeed -= GRAVITY_WHILE_JUMP * timeScale;
	}
	else
	{
		if (m_yoYo->IsAir())
		{
			m_fallSpeed -= GRAVITY_WHILE_ATTACK * timeScale;
		}
		else
		{
			m_fallSpeed -= GRAVITY_WHILE_FALL * timeScale;
		}
	}

	//落下速度加減
	if (m_fallSpeed < FALL_SPEED_MIN)m_fallSpeed = FALL_SPEED_MIN;

	//移動量加算
	pos += m_move * timeScale;

	//更新した座標の反映
	m_modelObj->m_transform.SetPos(pos);

	//落下ダメージ判定
	if (pos.y < FALL_LIMIT_HEIGHT)
	{
		m_damegedCallBack->Execute(true);
		InitMovement();
	}

	//被ダメージコールバック
	m_damegedCallBack->Update(arg_timeScale);

//ヨーヨー（攻撃）
	//投げる
	if (throwYoyoTrigger && m_yoYo->Throw(m_vecX))
	{
		m_fallSpeed = 0.0f;
		m_move.y = 0.0f;
	}
	//N攻撃
	if (neutralTrigger && m_yoYo->Neutral())
	{

	}

	//ヨーヨー
	m_yoYo->Update(arg_timeScale, m_vecX);

	//ヒットエフェクト
	m_hitEffect->Update(timeScale);
	m_counterHitEffect->Update(timeScale);

	//所持金コインUI
	m_coinUI.Update(m_coinVault.GetNum(), timeScale);

	//プレイヤーHPUI
	m_playerHp.Update(timeScale);

	//足場がない
	if (!m_bodyAABBCollider->GetIsHit())
	{
		//空中状態に
		OnAir();
	}
}

void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (m_damegedCallBack->GetIsDraw())
	{
		BasicDraw::Instance()->Draw(*arg_lightMgr.lock(), m_modelObj, *arg_cam.lock(), m_drawParam);
	}

	//ヨーヨー
	m_yoYo->Draw(arg_lightMgr, arg_cam, m_drawParam);
}

void Player::Draw2D(std::weak_ptr<Camera> arg_cam)
{
	//ヒットエフェクト
	m_hitEffect->Draw(arg_cam);
	m_counterHitEffect->Draw(arg_cam);

	//HP描画
	m_playerHp.Draw2D();

	//所持金コインUI
	m_coinUI.Draw2D();
}

Vec3<float> Player::GetCenterPos() const
{
	return m_modelObj->m_transform.GetPos();
}

void Player::SetDamageColldierActive(bool arg_active)
{
	m_bodySphereCollider->SetActive(arg_active);
}

bool Player::IsAttack() const
{
	return m_yoYo->IsInvincible();
}
