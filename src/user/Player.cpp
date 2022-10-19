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

Player::Player(std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	using namespace ConstParameter::Player;

	//モデル読み込み
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");

	//被ダメージヒットストップSE
	int onDamagedHitStopSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_hitStop.wav",0.5f);

	//被ダメージSE
	int onDamagedSE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_damage_onTrigger.wav",0.4f);

	//敵の踏みつけSE
	int onStepEnemySE = AudioApp::Instance()->LoadAudio("resource/user/sound/player_step.wav",0.8f);

	//ブロック破壊SE
	int blockBrokenSE = AudioApp::Instance()->LoadAudio("resource/user/sound/block_broken.wav", 0.5f);

	/*--- コライダー用プリミティブ生成 ---*/

	//モデル全体を覆う球
	std::shared_ptr<CollisionPrimitive>bodySphereCol = std::make_shared<CollisionSphere>(
		1.2f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -0.2f, 0.0f));

	//足元の当たり判定球
	std::shared_ptr<CollisionPrimitive>footSphereCol = std::make_shared<CollisionSphere>(
		1.0f,
		FIX_MODEL_CENTER_OFFSET + Vec3<float>(0.0f, -1.5f, 0.0f));

	/*--- コールバック生成 ---*/
	//被ダメージコールバック
	m_damegedCallBack = std::make_shared<DamagedCallBack>(this, onDamagedHitStopSE, onDamagedSE);
	//ジャンプ権回復コールバック
	m_callBackWithBlock = std::make_shared<CallBackWithBlock>(this, arg_collisionMgr, blockBrokenSE);

	/*--- コライダー生成（判定順） ---*/

	std::vector<std::shared_ptr<Collider>>colliders;
	
	//モデル全体を覆うコライダー
	{
		std::vector<std::shared_ptr<CollisionPrimitive>>coverModelPrimitiveArray =
		{
			bodySphereCol
		};
		m_bodyCollider = std::make_shared<Collider>();
		m_bodyCollider->Generate("Player_Body", "Player", coverModelPrimitiveArray);
		m_bodyCollider->SetParentObject(this);
		m_bodyCollider->SetParentTransform(&m_modelObj->m_transform);

		//被ダメージコールバックアタッチ
		m_bodyCollider->SetCallBack("Enemy", m_damegedCallBack.get());
		//ブロックに触れた際のコールバック
		m_bodyCollider->SetCallBack("Block", m_callBackWithBlock.get());
		colliders.emplace_back(m_bodyCollider);
	}

	/*--- コライダー配列登録 ---*/
	arg_collisionMgr.lock()->Register(colliders);
}

void Player::Init(std::weak_ptr<GameCamera>arg_cam)
{
	using namespace ConstParameter::Player;

	//HP初期化
	m_hp = MAX_HP;

	//スタート位置に移動
	m_modelObj->m_transform.SetPos(INIT_POS);

	//入力による移動方向初期化
	m_inputMoveVec = { 0,0,0 };

	//移動速度
	m_move = { 0,0,0 };

	//加速度
	m_accel = { 0,0,0 };

	//所持金リセット
	m_coinVault.Set(300000);

	//被ダメージコールバック
	m_damegedCallBack->Init(arg_cam);

	//攻撃ヒットエフェクト
	m_attackHitEffect.Init();
}

void Player::Update(std::weak_ptr<SlotMachine> arg_slotMachine, TimeScale& arg_timeScale)
{
	using namespace ConstParameter::Player;
	using namespace ConstParameter::Environment;

//入力情報取得
	const auto& input = *UsersInput::Instance();

	//入力設定
	switch (m_inputConfig)
	{
	//キーボード入力
	case Player::INPUT_CONFIG::KEY_BOARD:
	{
		//方向入力
		if (input.KeyInput(DIK_LEFT))m_inputMoveVec.x = -1.0f;
		else if (input.KeyInput(DIK_RIGHT))m_inputMoveVec.x = 1.0f;
		if (input.KeyInput(DIK_UP))m_inputMoveVec.y = 1.0f;
		else if (input.KeyInput(DIK_DOWN))m_inputMoveVec.y = -1.0f;
		break;
	}

	//コントローラー入力
	case Player::INPUT_CONFIG::CONTROLLER:
	{
		//方向入力
		auto inputVec = input.GetLeftStickVec(0);
		m_inputMoveVec.x = inputVec.x;
		m_inputMoveVec.y = -inputVec.y;	//Y軸逆
		break;
	}

	//その他の入力設定になっていた場合エラー
	default:
		assert(0);
		break;
	}

//入力情報を元に操作
	//加速
	m_move += m_accel;

	//移動量加算
	auto pos = m_modelObj->m_transform.GetPos();
	pos += m_move * arg_timeScale.GetTimeScale();

	//加速度減衰
	m_accel = KuroMath::Lerp(m_accel, m_inputMoveVec * INPUT_ACCEL_POWER, ACCEL_LERP_RATE);

	//移動量減衰
	m_move = KuroMath::Lerp(m_move, Vec3<float>(0, 0, 0), MOVE_DAMP_RATE);

	//押し戻し（床）
	if (pos.y < 0.0f)
	{
	}

	//押し戻し（ステージ端）
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

	//攻撃ヒットエフェクト
	m_attackHitEffect.Update(arg_timeScale.GetTimeScale());
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam)
{
	if (!m_damegedCallBack->GetIsDraw())return;

	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *arg_cam.lock(), 1.0f, AlphaBlendMode_None);
}

void Player::EffectDraw(std::weak_ptr<Camera> arg_cam)
{
	m_attackHitEffect.Draw(arg_cam);
}

Vec3<float> Player::GetCenterPos() const
{
	return m_modelObj->m_transform.GetPos() + ConstParameter::Player::FIX_MODEL_CENTER_OFFSET;
}

void Player::DamagedCallBack::OnCollisionTrigger(const Vec3<float>& arg_inter, 
	std::weak_ptr<Collider>arg_otherCollider)
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

void Player::CallBackWithBlock::OnCollisionTrigger(const Vec3<float>& arg_inter, std::weak_ptr<Collider> arg_otherCollider)
{
	m_parent->m_attackHitEffect.Emit(arg_inter);
	AudioApp::Instance()->PlayWaveDelay(m_brokenSE, 3);

	auto block = arg_otherCollider.lock()->GetParentObject<Block>();
	auto accelVec = block->m_transform.GetPos() - m_parent->GetCenterPos();

	const float ACCEL_POWER = 0.7f;
	m_parent->m_accel = accelVec.GetNormal() * ACCEL_POWER;
}