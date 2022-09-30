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

Player::Player(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	//モデル読み込み
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	//BETのSE読み込み
	m_betSE = AudioApp::Instance()->LoadAudio("resource/user/sound/coin.wav",0.15f);

	/*--- コライダー用プリミティブ生成 ---*/
	//モデル中央に合わせるためのオフセット値
	const Vec3<float>FIX_MODEL_CENTER_OFFSET = { 0.0f,2.0f,ConstParameter::Environment::FIELD_DEPTH_MODEL_OFFSET };

	//モデル全体を覆う球
	std::shared_ptr<CollisionPrimitive>bodySphereCol = std::make_shared<CollisionSphere>(
		1.4f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -0.3f, 0.0f),
		&m_modelObj->m_transform);

	/*--- コールバック生成 ---*/
	//被ダメージコールバック
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this);

	/*--- モデル全体を覆うコライダー生成 ---*/
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			bodySphereCol
		};
		std::shared_ptr<Collider>bodyCollider = std::make_shared<Collider>("Player_Damaged", coverModelPrimitiveArray);

		//被ダメージコールバックアタッチ
		bodyCollider->SetCallBack(m_damegedCallBack, arg_collisionMgr.lock()->GetAttribute("Enemy"));
		m_colliders.emplace_back(bodyCollider);
	}

	/*--- コライダー配列登録 ---*/
	arg_collisionMgr.lock()->Register("Player", m_colliders);
}

void Player::Init()
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
	m_damegedCallBack->Init();
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, const TimeScale& arg_timeScale)
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
		m_fallSpeed += ConstParameter::Player::JUMP_POWER;
		m_isOnGround = false;
	}

	//落下（ジャンプ中と落下中で重力変化、素早くジャンプ → ゆっくり降下）
	m_move.y += m_fallSpeed;
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
	m_damegedCallBack->Update(arg_timeScale.GetTimeScale());
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
}

void Player::DamagedCallBack::OnCollision(const Vec3<float>& arg_inter, const unsigned char& arg_otherAttribute, const CollisionManager& arg_collisionMgr)
{
	//無敵時間中か
	if (!m_invincibleTimer.IsTimeUp())return;

	m_parent->m_hp--;
	printf("Player : Damaged : remain hp %d\n", m_parent->m_hp);

	m_invincibleTimer.Reset(ConstParameter::Player::INVINCIBLE_TIME_WHEN_DAMAGED);
}