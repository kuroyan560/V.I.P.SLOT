#pragma once
#include<array>
#include<vector>
#include<memory>
#include"Transform.h"
#include"Timer.h"
class ModelObject;
class CollisionManager;
class CollisionSphere;
class Collider;
class TimeScale;
class LightManager;
class Camera;

class YoYo
{
	//[[f
	std::shared_ptr<ModelObject>m_modelObj;

	//ό«wθpgXtH[
	Transform m_vecTransform;

	//RC_[
	//NU
	std::shared_ptr<CollisionSphere>m_neutralColSphere;
	std::shared_ptr<Collider>m_neutralCol;
	//±
	std::shared_ptr<CollisionSphere>m_throwColSphere;
	std::shared_ptr<Collider>m_throwCol;


	//σΤJΪ
	enum STATUS
	{
		THROW_0,	//°_0
		THROW_1,	//°_1
		THROW_2,	//°_2
		NEUTRAL,	//NU
		STATUS_NUM,
		HAND
	}m_status;

	//Xe[^XIΉvͺ^C}[
	std::array<int, STATUS_NUM>m_finishInterval = { 90,90,90,90 };

	//Xe[^XIΉvͺp^C}[
	Timer m_timer;

public:

	YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform);

	/// <summary>
	/// [[ΜΕθp[^έθiϊ»j
	/// </summary>
	/// <param name="arg_neutralColSphereRadius">½θ»θpΜΌaiNUj</param>
	/// <param name="arg_throwColSphereRadius">½θ»θpΜΌai±j</param>
	void Awake(float arg_neutralColSphereRadius, float arg_throwColSphereRadius);

	//ϊ»
	void Init();
	//XV
	void Update(const TimeScale& arg_timeScale);
	//`ζ
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//imguifobO
	void AddImguiDebugItem();

	//°ι
	void Throw(Vec2<float>arg_vec);

	//U©
	bool IsActive() { return m_status != HAND; }
};