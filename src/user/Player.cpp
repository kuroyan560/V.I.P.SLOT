#include "Player.h"
#include"Object.h"

Player::Player()
{
	//モデル読み込み
	m_modelObj = std::make_shared<ModelObject>("resource/user/model/", "player.glb");
}

void Player::Init()
{
	//スタート位置に移動
	m_modelObj->m_transform.SetPos(m_startPos);
}

void Player::Update()
{
}

#include"DrawFunc3D.h"
void Player::Draw(std::weak_ptr<LightManager>LigMgr, std::weak_ptr<Camera>Cam)
{
	//DrawFunc3D::DrawADSShadingModel(*LigMgr.lock(), m_modelObj, *Cam.lock(), AlphaBlendMode_None);
	DrawFunc3D::DrawNonShadingModel(m_modelObj, *Cam.lock(), 1.0f, AlphaBlendMode_None);

}
