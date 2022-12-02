#include "Scaffold.h"
#include"Importer.h"
#include"ConstParameters.h"

std::shared_ptr<Model>Scaffold::s_model;

Scaffold::Scaffold()
{
	if (!s_model)
	{
		s_model = Importer::Instance()->LoadModel("resource/user/model/", "scaffold.glb");
	}
}

void Scaffold::Init(float arg_posX, float arg_posY,float arg_width)
{
	Vec3<float>pos = { arg_posX,arg_posY,ConstParameter::Environment::FIELD_FLOOR_POS.z };
	m_transform.SetPos(pos);
	m_width = arg_width;
	m_transform.SetScale({ arg_width,2.0f,1.0f });
}

#include"BasicDraw.h"
void Scaffold::Draw(std::weak_ptr<LightManager> arg_lightMgr, std::weak_ptr<Camera> arg_cam)
{
	BasicDraw::Instance()->Draw(*arg_lightMgr.lock(), s_model, m_transform, *arg_cam.lock());
	//DrawFunc3D::DrawNonShadingModel(s_model, m_transform, *arg_cam.lock(), 1.0f, nullptr, AlphaBlendMode_None);
}