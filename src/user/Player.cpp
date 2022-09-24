#include "Player.h"
#include"Object.h"

Player::Player()
{
	//モデル読み込み
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");
}

void Player::Init()
{
}

void Player::Update()
{
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam)
{
	DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
}
