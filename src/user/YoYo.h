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
	//ƒˆ[ƒˆ[ƒ‚ƒfƒ‹
	std::shared_ptr<ModelObject>m_modelObj;

	//Œü‚«w’è—pƒgƒ‰ƒ“ƒXƒtƒH[ƒ€
	Transform m_vecTransform;

	//“Š‚°‚é•ûŒü
	enum THROW_VEC { LEFT, LEFT_UP, RIGHT_UP, RIGHT, THROW_VEC_NUM };

	//ƒJƒvƒZƒ‹‚ÌŒ`‚ğ¬‚·“–‚½‚è”»’è—p‹…”z—ñ
	std::vector<std::shared_ptr<CollisionSphere>>m_capsuleSphereArray;
	//‹…‚ª˜A‚È‚Á‚Ä‚Å‚«‚½ƒJƒvƒZƒ‹ƒRƒ‰ƒCƒ_[
	//¦ƒJƒvƒZƒ‹‚ÌCollisionPrimitive‚ÌÀ‘•ŠÔ‚ª‚à‚Á‚½‚¢‚È‚¢‚Ì‚ÅA‚Æ‚è‚ ‚¦‚¸‹…‚ÅB
	std::shared_ptr<Collider>m_throwCol;

	//ó‘Ô‘JˆÚ
	enum STATUS
	{
		THROW_0,	//“Š‚°_0
		THROW_1,	//“Š‚°_1
		THROW_2,	//“Š‚°_2
		NEUTRAL,	//NUŒ‚
		STATUS_NUM,
		HAND
	}m_status;

	//ƒXƒe[ƒ^ƒXI—¹Œv‘ªƒ^ƒCƒ}[
	std::array<int, STATUS_NUM>m_finishInterval = { 90,90,90,90 };

	//ƒXƒe[ƒ^ƒXI—¹Œv‘ª—pƒ^ƒCƒ}[
	Timer m_timer;

public:

	YoYo(std::weak_ptr<CollisionManager>arg_collisionMgr, Transform* arg_playerTransform);

	/// <summary>
	/// ƒˆ[ƒˆ[‚ÌŒÅ’èƒpƒ‰ƒ[ƒ^İ’èi‚‰Šú‰»j
	/// </summary>
	/// <param name="arg_length">ƒˆ[ƒˆ[‚Ì’·‚³</param>
	/// <param name="arg_colSphereRadius">“–‚½‚è”»’è—p‹…‚Ì”¼Œa</param>
	void Awake(float arg_length,float arg_colSphereRadius);

	//‰Šú‰»
	void Init();
	//XV
	void Update(const TimeScale& arg_timeScale);
	//•`‰æ
	void Draw(std::weak_ptr<LightManager>arg_lightMgr, std::weak_ptr<Camera>arg_cam);

	//imguiƒfƒoƒbƒO
	void AddImguiDebugItem();

	//“Š‚°‚é
	void Throw(Vec2<float>arg_vec);

	//ï¿½Uï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
	bool IsActive() { return m_status != HAND; }
};