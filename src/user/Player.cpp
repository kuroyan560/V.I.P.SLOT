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

	//移動速度
	m_move = { 0,0,0 };
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

	//移動
	const float MOVE_SPEED = 0.3f;
	const float MOVE_LERP_RATE = 0.3f;
	if (0.0f < stickVec.x)
	{
		m_move.x = KuroMath::Lerp(m_move.x, MOVE_SPEED, MOVE_LERP_RATE);
	}
	else if (stickVec.x < 0.0f)
	{
		m_move.x = KuroMath::Lerp(m_move.x, -MOVE_SPEED, MOVE_LERP_RATE);
	}
	else
	{
		m_move.x = KuroMath::Lerp(m_move.x, 0.0f, MOVE_LERP_RATE);
	}

	//現在の座標取得
	auto pos = m_modelObj->m_transform.GetPos();
	//移動量を加算して反映
	m_modelObj->m_transform.SetPos(pos + m_move);
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam)
{
	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *Cam.lock(), 1.0f, AlphaBlendMode_None);

}
