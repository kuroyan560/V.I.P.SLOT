#include "Scaffold.h"
#include"Importer.h"
#include"ConstParameters.h"
#include"Collision.h"
#include"Model.h"
#include"Collider.h"
#include"CollisionManager.h"

std::shared_ptr<Model>Scaffold::s_model;

Scaffold::Scaffold()
{
	if (!s_model)
	{
		s_model = Importer::Instance()->LoadModel("resource/user/model/", "scaffold.glb");
	}

	//コライダー生成
	auto colPimiritive = std::make_shared<CollisionAABB>(s_model->GetAllMeshPosMinMax());
	m_collider = std::make_shared<Collider>();
	m_collider->Generate("Scaffold", { "Scaffold" }, { colPimiritive });
	m_collider->SetParentTransform(&m_transform);

}

void Scaffold::Init(float arg_posX, float arg_posY, float arg_width, std::weak_ptr<CollisionManager>arg_collisionMgr)
{
	Vec3<float>pos = { arg_posX,arg_posY,ConstParameter::Environment::FIELD_FLOOR_POS.z };
	m_transform.SetPos(pos);
	m_width = arg_width;
	m_transform.SetScale({ arg_width,2.0f,1.0f });
	arg_collisionMgr.lock()->Register(m_collider);
}

#include"BasicDraw.h"
void Scaffold::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	BasicDraw::Instance()->Draw(*arg_lightMgr.lock(), s_model, m_transform, *arg_cam.lock());
	//DrawFunc3D::DrawNonShadingModel(s_model, m_transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
}

void Scaffold::Disappear(std::weak_ptr<CollisionManager> arg_collisionMgr)
{
	arg_collisionMgr.lock()->Remove(m_collider);
}
