#include "Player.h"
#include"Object.h"
#include"UsersInput.h"

Player::Player()
{
	//モデル読み込み
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");
}

void Player::Init()
{
	//スタート位置に移動
	m_modelObj->m_transform.SetPos(m_startPos);

	//座標角度初期化
	m_posAngle = Angle(0);

	//座標角移動量初期化
	m_moveAngle = Angle(0);

	//落下速度初期化
	m_fallAccel = 0.0f;

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
	const Angle ANGLE_SPEED = Angle::ConvertToRadian(0.85f);
	const float MOVE_LERP_RATE = 0.3f;
	if (0.0f < stickVec.x)
	{
		m_moveAngle = KuroMath::Lerp(m_moveAngle, -ANGLE_SPEED, MOVE_LERP_RATE);
	}
	else if (stickVec.x < 0.0f)
	{
		m_moveAngle = KuroMath::Lerp(m_moveAngle, ANGLE_SPEED, MOVE_LERP_RATE);
	}
	else
	{
		m_moveAngle = KuroMath::Lerp(m_moveAngle, 0.0f, MOVE_LERP_RATE);
	}

	//ジャンプ
	const float JUMP_POWER = 0.62f;
	if (jumpTrigger && m_isOnGround)
	{
		m_fallAccel = JUMP_POWER;
		m_isOnGround = false;
	}

	//落下加速
	m_fallSpeed += m_fallAccel;

	//落下速度計算（ジャンプ中と落下中で重力変化、素早くジャンプ → ゆっくり降下）
	const float STRONG_GRAVITY = 0.34f;
	const float WEAK_GRAVITY = 0.00005f;
	if (0.0f < m_fallAccel)
	{
		m_fallAccel -= STRONG_GRAVITY;
	}
	else
	{
		m_fallAccel -= WEAK_GRAVITY;
	}
	//落下加速度下限
	const float FALL_SPEED_MIN = -0.2f;
	if (m_fallAccel < FALL_SPEED_MIN)m_fallAccel = FALL_SPEED_MIN;

	//現在の座標取得
	auto pos = m_modelObj->m_transform.GetPos();
	//落下を反映
	pos.y += m_fallSpeed;
	//横移動の反映
	m_posAngle += m_moveAngle;
	auto xzPos = KuroMath::TransformVec3(m_startPos, { 0,1,0 }, m_posAngle);
	pos.x = xzPos.x;
	pos.z = xzPos.z;

	//押し戻し
	if (pos.y < 0.0f)
	{
		pos.y = 0.0f;
		m_fallAccel = 0.0f;
		m_fallSpeed = 0.0f;
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
