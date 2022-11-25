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

void Player::Jump(Vec3<float>* arg_rockOnPos)
{
	m_move.y = 0.0f;
	m_fallSpeed = ConstParameter::Player::JUMP_POWER;
	m_isOnGround = false;
	m_isOnScaffold = false;
}

void Player::OnLanding(bool arg_isGround)
{
	m_fallSpeed = 0.0f;
	m_move.y = 0.0f;
	if (arg_isGround)m_isOnGround = true;
	else m_isOnScaffold = true;
}

Player::Player()
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
	m_hitEffect->Set("resource/user/img/hitEffect.png", 5, { 5,1 }, { 6.0f,6.0f }, 3);

	//攻撃コールバック
	m_normalAttackCallBack = std::make_shared<PlayersNormalAttack>(&m_offensive, m_hitEffect, enemyHitSE, enemyKillSE);
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
	m_bodySphereCol = std::make_shared<CollisionSphere>(
		1.4f,
		Vec3<float>(0.0f, -0.2f, 0.0f));

	//足元の当たり判定球
	std::shared_ptr<CollisionPrimitive>footSphereCol = std::make_shared<CollisionSphere>(
		1.0f,
		Vec3<float>(0.0f, -1.5f, 0.0f));

	/*--- コールバック生成 ---*/
	//パリィ攻撃コールバック
	m_parryAttackCallBack = std::make_shared<PlayersParryAttack>(&m_offensive, arg_objMgr, arg_collisionMgr, parrySE);
	//被ダメージコールバック
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, arg_cam, onDamagedHitStopSE, onDamagedSE);

	/*--- コライダー生成（判定順） ---*/

	std::vector<std::shared_ptr<Collider>>colliders;

	//モデル全体を覆うコライダー
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			m_bodySphereCol
		};
		m_bodyCollider = std::make_shared<Collider>();
		m_bodyCollider->Generate("Player_Body", { "Player" }, coverModelPrimitiveArray);
		m_bodyCollider->SetParentObject(this);
		m_bodyCollider->SetParentTransform(&m_modelObj->m_transform);

		//被ダメージコールバックアタッチ
		m_bodyCollider->SetCallBack("Enemy", m_damegedCallBack.get());
		m_bodyCollider->SetCallBack("Enemy_Attack", m_damegedCallBack.get());
		colliders.emplace_back(m_bodyCollider);
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

void Player::Init(int arg_initHp, int arg_initCoinNum)
{
	using namespace ConstParameter::Player;

	//HP初期化
	m_hp = arg_initHp;

	//所持金リセット
	m_coinVault.Set(arg_initCoinNum);

	//スタート位置に移動
	m_modelObj->m_transform.SetPos(INIT_POS);

	m_oldPos = INIT_POS;

	//移動速度
	m_move = { 0,0,0 };

	//落下速度初期化
	m_fallSpeed = 0.0f;

	//接地フラグ初期化
	m_isOnGround = true;

	//所持金リセット
	m_coinVault.Set(0);

	//被ダメージコールバック
	m_damegedCallBack->Init();

	//ヨーヨー
	m_yoYo->Init();

	//デフォルト右向き
	m_vecX = 1.0f;

	//足場から降りる処理初期化
	m_stepDownTimer.Reset(3);
	m_stepDown = false;

	//ヒットエフェクト
	m_hitEffect->Init();
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
	m_stepDownTimer.UpdateTimer(timeScale);
	//足場から降りる
	bool stepDownInput = (-moveInput.y < 0.0f);
	if (stepDownInput)
	{
		if (!m_stepDown && m_isOnScaffold)
		{
			m_stepDownTimer.Reset();
			m_stepDown = true;
			m_isOnScaffold = false;
			m_fallSpeed = m_stepDownFallSpeed;
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

	//押し戻し（床）
	if (pos.y < FIELD_FLOOR_TOP_SURFACE_HEIGHT + MODEL_SIZE.y / 2.0f)
	{
		pos.y = FIELD_FLOOR_TOP_SURFACE_HEIGHT + MODEL_SIZE.y / 2.0f;
		OnLanding(true);
	}

	//押し戻し（ステージ端）
	const float FIELD_WIDTH_HALF = FIELD_FLOOR_SIZE.x / 2.0f;
	if (pos.x < -FIELD_WIDTH_HALF)
	{
		pos.x = -FIELD_WIDTH_HALF;
		m_move.x = 0.0f;
	}
	else if(FIELD_WIDTH_HALF < pos.x)
	{
		pos.x = FIELD_WIDTH_HALF;
		m_move.x = 0.0f;
	}

	//更新した座標の反映
	m_modelObj->m_transform.SetPos(pos);

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
}

void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (m_damegedCallBack->GetIsDraw())
	{
		BasicDraw::Draw(*arg_lightMgr.lock(), m_modelObj, *arg_cam.lock(), m_toonParams);
	}

	//ヨーヨー
	m_yoYo->Draw(arg_lightMgr, arg_cam, m_toonParams);
}

void Player::Draw2D(std::weak_ptr<Camera> arg_cam)
{
	//ヒットエフェクト
	m_hitEffect->Draw(arg_cam);
}

#include"imguiApp.h"
void Player::ImguiDebug()
{
	ImGui::Begin("Player");
	m_toonParams.ImguiDebugItem();
	ImGui::Text("Coin : { %d }", m_coinVault.GetNum());
	ImGui::Text("Hp : { %d }", m_hp);
	m_yoYo->AddImguiDebugItem();
	ImGui::End();
}

void Player::HitCheckWithScaffold(const std::weak_ptr<Scaffold> arg_scaffold)
{
	//足場との判定を切っている
	if (!m_stepDownTimer.IsTimeUp())return;

	//自身の座標取得
	auto pos = m_modelObj->m_transform.GetPos();

	//落下していない
	if (m_oldPos.y <= pos.y)return;

	//足場ポインタ取得
	auto scaffold = arg_scaffold.lock();

	//１フレーム前の座標
	auto oldPos = m_oldPos;

	//当たり判定
	auto scaffoldPos = scaffold->GetPos();
	auto scaffoldNormal = scaffold->GetNormal();

	float myRadius = m_bodySphereCol->m_radius;
	auto v1 = pos + Vec3<float>(0.0f, -myRadius, 0.0f) - scaffoldPos;
	auto v2 = oldPos+ Vec3<float>(0.0f, -myRadius, 0.0f) - scaffoldPos;

	//足場面と衝突してない
	if (0.0f < v1.Dot(scaffoldNormal) * v2.Dot(scaffoldNormal))return;

	//足場面までの距離
	auto d1 = abs(scaffoldNormal.Dot(v1));
	auto d2 = abs(scaffoldNormal.Dot(v2));
	float a = d1 / (d1 + d2);
	auto v3 = v1 * (1 - a) + v2 * a;

	//衝突点
	auto inter = v3 + scaffoldPos;

	//衝突点が足場面上に含まれているか
	auto scaffoldWidth = scaffold->GetWidth();
	//足場の各角
	Vec3<float>p1 = scaffoldPos + Vec3<float>(-scaffoldWidth / 2.0f, 0.0f, 0.5f);
	Vec3<float>p2 = scaffoldPos + Vec3<float>(scaffoldWidth / 2.0f, 0.0f, 0.5f);
	Vec3<float>p3 = scaffoldPos + Vec3<float>(-scaffoldWidth / 2.0f, 0.0f, -0.5f);
	Vec3<float>p4 = scaffoldPos + Vec3<float>(scaffoldWidth / 2.0f, 0.0f, -0.5f);
	
	if ((inter - p1).Cross(p1 - p2).GetNormal() != scaffoldNormal)return;
	if ((inter - p2).Cross(p2 - p4).GetNormal() != scaffoldNormal)return;
	if ((inter - p4).Cross(p4 - p3).GetNormal() != scaffoldNormal)return;
	if ((inter - p3).Cross(p3 - p1).GetNormal() != scaffoldNormal)return;

	//押し戻し
	pos.y = inter.y + myRadius + 0.01f;
	m_modelObj->m_transform.SetPos(pos);

	//着地時の処理
	OnLanding(false);
}

Vec3<float> Player::GetCenterPos() const
{
	return m_modelObj->m_transform.GetPos();
}

bool Player::IsAttack() const
{
	return m_yoYo->IsInvincible();
}
