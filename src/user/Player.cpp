#include "Player.h"
#include"Object.h"
#include"UsersInput.h"
#include"ConstParameters.h"

Player::Player()
{
	//モデル読み込み
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");
}

void Player::Init()
{
	//スタート位置に移動
	m_modelObj->m_transform.SetPos(ConstParameter::Player::INIT_POS);

	//移動速度
	m_move = { 0,0,0 };

	//落下速度初期化
	m_fallSpeed = 0.0f;

	//接地フラグ初期化
	m_isOnGround = true;
}

void Player::Update()
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
		m_fallSpeed -= ConstParameter::Player::STRONG_GRAVITY;
	}
	else
	{
		m_fallSpeed -= ConstParameter::Player::WEAK_GRAVITY;
	}

	//落下速度加減
	if (m_fallSpeed < ConstParameter::Player::FALL_SPEED_MIN)m_fallSpeed = ConstParameter::Player::FALL_SPEED_MIN;

	//移動量加算
	auto pos = m_modelObj->m_transform.GetPos();
	pos += m_move;

	//押し戻し
	if (pos.y < 0.0f)
	{
		pos.y = 0.0f;
		m_fallSpeed = 0.0f;
		m_move.y = 0.0f;
		m_isOnGround = true;
	}

	//更新した座標の反映
	m_modelObj->m_transform.SetPos(pos);
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam)
{
	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *Cam.lock(), 1.0f, AlphaBlendMode_None);

}
