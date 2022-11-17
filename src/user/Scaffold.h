#pragma once
#include"Vec.h"
#include"Transform.h"
#include<memory>
class LightManager;
class Camera;
class Model;

//足場
class Scaffold
{
	//モデルオブジェクト
	static std::shared_ptr<Model>s_model;
	//トランスフォーム
	Transform m_transform;

	float m_width = 1.0f;
	Vec3<float>m_normal = { 0.0f,1.0f,0.0f };

public:
	Scaffold();
	void Init(float arg_posX, float arg_posY, float arg_width);
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	Vec3<float>GetPos()const { return m_transform.GetPos(); }
	const float& GetWidth()const { return m_width; }
	const Vec3<float>& GetNormal()const { return m_normal; }
};